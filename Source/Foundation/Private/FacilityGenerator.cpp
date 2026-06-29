#include "FacilityGenerator.h"
#include "RoomConnector.h"
#include "SCPEntityBase.h"
#include "SCP173.h"
#include "SCP096.h"
#include "SCP106.h"
#include "SCP682.h"
#include "ExitZone.h"
#include "KeycardPickup.h"
#include "ContainmentTerminal.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

AFacilityGenerator::AFacilityGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	ExitZoneClass = AExitZone::StaticClass();
	KeycardClass = AKeycardPickup::StaticClass();
	TerminalClass = AContainmentTerminal::StaticClass();
}

FVector AFacilityGenerator::GetStartLocation() const
{
	return GetActorLocation() + FVector(0, 0, 120.f);
}

ERoomShape AFacilityGenerator::RandomShape()
{
	const int32 Pick = Rng.RandRange(0, 99);
	if (Pick < 40) return ERoomShape::Straight;
	if (Pick < 65) return ERoomShape::Corner;
	if (Pick < 85) return ERoomShape::TJunction;
	if (Pick < 95) return ERoomShape::Cross;
	return ERoomShape::DeadEnd;
}

void AFacilityGenerator::Generate()
{
	Rng.Initialize(RandomSeed != 0 ? RandomSeed : FMath::Rand());
	PlacedRooms.Empty();
	PlacedBounds.Empty();

	CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));

	if (SCPClasses.Num() == 0)
	{
		SCPClasses.Add(ASCP173::StaticClass());
		SCPClasses.Add(ASCP096::StaticClass());
		SCPClasses.Add(ASCP106::StaticClass());
		SCPClasses.Add(ASCP682::StaticClass());
		SCPClasses.Add(ASCPEntityBase::StaticClass());
	}

	TArray<FOpenConnector> Open;

	ARoomBase* Start = SpawnShapedRoom(ERoomShape::Spawn, GetActorTransform());
	if (!Start)
	{
		return;
	}
	RegisterRoom(Start, Open);

	int32 Guard = 0;
	while (PlacedRooms.Num() < TargetRoomCount && Open.Num() > 0 && Guard < TargetRoomCount * 40)
	{
		++Guard;
		const int32 Idx = Rng.RandRange(0, Open.Num() - 1);
		FOpenConnector Target = Open[Idx];
		Open.RemoveAtSwap(Idx);

		TArray<FOpenConnector> NewOpen;
		if (TryPlaceRoom(Target, NewOpen))
		{
			Open.Append(NewOpen);
		}
		else
		{
			CapConnector(Target);
		}
	}

	// Seal any remaining openings so the facility is enclosed.
	for (const FOpenConnector& Leftover : Open)
	{
		CapConnector(Leftover);
	}

	PopulateFacility();

	UE_LOG(LogTemp, Log, TEXT("[Facility] Generated %d rooms."), PlacedRooms.Num());
}

ARoomBase* AFacilityGenerator::SpawnShapedRoom(ERoomShape ShapeToUse, const FTransform& Xform)
{
	if (UWorld* World = GetWorld())
	{
		ARoomBase* Room = World->SpawnActorDeferred<ARoomBase>(ARoomBase::StaticClass(), Xform);
		if (Room)
		{
			Room->Shape = ShapeToUse;
			UGameplayStatics::FinishSpawningActor(Room, Xform);
		}
		return Room;
	}
	return nullptr;
}

void AFacilityGenerator::RegisterRoom(ARoomBase* Room, TArray<FOpenConnector>& OpenList)
{
	PlacedRooms.Add(Room);
	PlacedBounds.Add(ComputeWorldBounds(Room, Room->GetActorTransform()));

	for (int32 i = 0; i < Room->Connectors.Num(); ++i)
	{
		URoomConnector* Conn = Room->Connectors[i];
		if (Conn && !Conn->bUsed)
		{
			FOpenConnector Open;
			Open.Room = Room;
			Open.ConnectorIndex = i;
			Open.WorldXform = Conn->GetComponentTransform();
			OpenList.Add(Open);
		}
	}
}

bool AFacilityGenerator::TryPlaceRoom(const FOpenConnector& Target, TArray<FOpenConnector>& OutNewOpen)
{
	if (!Target.Room || !Target.Room->Connectors.IsValidIndex(Target.ConnectorIndex))
	{
		return false;
	}
	if (Target.Room->Connectors[Target.ConnectorIndex]->bUsed)
	{
		return false;
	}

	// Desired pose for the mating connector: same location, facing 180 from the target.
	const FTransform Desired(
		Target.WorldXform.Rotator() + FRotator(0.f, 180.f, 0.f),
		Target.WorldXform.GetLocation());

	const int32 Attempts = 8;
	for (int32 a = 0; a < Attempts; ++a)
	{
		ARoomBase* Candidate = SpawnShapedRoom(RandomShape(), FTransform::Identity);
		if (!Candidate || Candidate->Connectors.Num() == 0)
		{
			if (Candidate) { Candidate->Destroy(); }
			continue;
		}

		const int32 Ci = Rng.RandRange(0, Candidate->Connectors.Num() - 1);
		URoomConnector* MateConn = Candidate->Connectors[Ci];

		const FTransform ConnRelative =
			MateConn->GetComponentTransform().GetRelativeTransform(Candidate->GetActorTransform());

		const FTransform ActorXform = ConnRelative.Inverse() * Desired;

		const FBox Bounds = ComputeWorldBounds(Candidate, ActorXform);
		if (OverlapsExisting(Bounds))
		{
			Candidate->Destroy();
			continue;
		}

		Candidate->SetActorTransform(ActorXform);

		Target.Room->Connectors[Target.ConnectorIndex]->bUsed = true;
		MateConn->bUsed = true;

		PlacedRooms.Add(Candidate);
		PlacedBounds.Add(Bounds);

		for (int32 j = 0; j < Candidate->Connectors.Num(); ++j)
		{
			URoomConnector* Conn = Candidate->Connectors[j];
			if (Conn && !Conn->bUsed)
			{
				FOpenConnector Open;
				Open.Room = Candidate;
				Open.ConnectorIndex = j;
				Open.WorldXform = Conn->GetComponentTransform();
				OutNewOpen.Add(Open);
			}
		}
		return true;
	}

	return false;
}

FBox AFacilityGenerator::ComputeWorldBounds(ARoomBase* Room, const FTransform& Xform) const
{
	const FBox Local = Room->GetLocalBounds();
	FBox Result(ForceInit);
	for (int32 c = 0; c < 8; ++c)
	{
		const FVector Corner(
			(c & 1) ? Local.Max.X : Local.Min.X,
			(c & 2) ? Local.Max.Y : Local.Min.Y,
			(c & 4) ? Local.Max.Z : Local.Min.Z);
		Result += Xform.TransformPosition(Corner);
	}
	return Result;
}

bool AFacilityGenerator::OverlapsExisting(const FBox& Box) const
{
	const FBox Shrunk = Box.ExpandBy(-60.f); // allow shared walls
	for (const FBox& Existing : PlacedBounds)
	{
		if (Shrunk.Intersect(Existing.ExpandBy(-60.f)))
		{
			return true;
		}
	}
	return false;
}

void AFacilityGenerator::CapConnector(const FOpenConnector& Open)
{
	if (!CubeMesh || !GetWorld())
	{
		return;
	}

	AActor* Cap = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), Open.WorldXform);
	if (!Cap)
	{
		return;
	}

	UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(Cap);
	Cap->SetRootComponent(Mesh);
	Mesh->RegisterComponent();
	Mesh->SetStaticMesh(CubeMesh);
	Mesh->SetWorldTransform(Open.WorldXform);
	Mesh->AddWorldOffset(FVector(0, 0, 150.f));
	Mesh->SetWorldScale3D(FVector(20.f, 280.f, 320.f) / 100.f);
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void AFacilityGenerator::PopulateFacility()
{
	if (PlacedRooms.Num() < 2)
	{
		return;
	}

	const FVector StartLoc = PlacedRooms[0]->GetActorLocation();

	// Exit zone in the farthest room from the start.
	ARoomBase* FarRoom = PlacedRooms[1];
	float FarDist = 0.f;
	for (ARoomBase* R : PlacedRooms)
	{
		const float D = FVector::Dist(StartLoc, R->GetActorLocation());
		if (D > FarDist)
		{
			FarDist = D;
			FarRoom = R;
		}
	}
	if (ExitZoneClass)
	{
		GetWorld()->SpawnActor<AExitZone>(ExitZoneClass, FarRoom->GetActorLocation() + FVector(0, 0, 60.f), FRotator::ZeroRotator);
	}

	// Keycards scattered in random rooms.
	if (KeycardClass)
	{
		for (int32 i = 0; i < KeycardCount; ++i)
		{
			ARoomBase* R = PlacedRooms[Rng.RandRange(1, PlacedRooms.Num() - 1)];
			AKeycardPickup* Card = GetWorld()->SpawnActor<AKeycardPickup>(
				KeycardClass, R->GetActorLocation() + FVector(0, 0, 90.f), FRotator::ZeroRotator);
			if (Card)
			{
				Card->KeycardLevel = Rng.RandRange(1, 3);
			}
		}
	}

	// Containment terminals scattered through the facility.
	if (TerminalClass)
	{
		for (int32 i = 0; i < TerminalCount; ++i)
		{
			ARoomBase* R = PlacedRooms[Rng.RandRange(1, PlacedRooms.Num() - 1)];
			GetWorld()->SpawnActor<AContainmentTerminal>(
				TerminalClass, R->GetActorLocation() + FVector(200.f, 0.f, 70.f), FRotator::ZeroRotator);
		}
	}

	// SCP entities, kept away from the start room.
	for (int32 i = 0; i < SCPCount && SCPClasses.Num() > 0; ++i)
	{
		TSubclassOf<ASCPEntityBase> Cls = SCPClasses[i % SCPClasses.Num()];
		if (!Cls) { continue; }

		ARoomBase* R = PlacedRooms[Rng.RandRange(PlacedRooms.Num() / 2, PlacedRooms.Num() - 1)];
		GetWorld()->SpawnActor<ASCPEntityBase>(Cls, R->GetActorLocation() + FVector(0, 0, 100.f), FRotator::ZeroRotator);
	}
}
