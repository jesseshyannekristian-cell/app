#include "RoomBase.h"
#include "RoomConnector.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

static const float WallHeight = 400.f;
static const float WallThickness = 20.f;
static const float DoorWidth = 240.f;
static const float DoorHeight = 300.f;

ARoomBase::ARoomBase()
{
	PrimaryActorTick.bCanEverTick = false;

	RoomRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RoomRoot"));
	SetRootComponent(RoomRoot);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		CubeMesh = CubeFinder.Object;
	}
}

void ARoomBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	BuildRoom();
}

FVector ARoomBase::SideDir(int32 SideIndex)
{
	switch (SideIndex)
	{
	case 0: return FVector(1, 0, 0);
	case 1: return FVector(0, 1, 0);
	case 2: return FVector(-1, 0, 0);
	default: return FVector(0, -1, 0);
	}
}

float ARoomBase::SideYaw(int32 SideIndex)
{
	return 90.f * SideIndex; // 0,90,180,270
}

bool ARoomBase::SideHasConnector(int32 SideIndex) const
{
	switch (Shape)
	{
	case ERoomShape::Straight:	return SideIndex == 0 || SideIndex == 2;
	case ERoomShape::Corner:	return SideIndex == 0 || SideIndex == 1;
	case ERoomShape::TJunction:	return SideIndex == 0 || SideIndex == 1 || SideIndex == 3;
	case ERoomShape::Cross:		return true;
	case ERoomShape::DeadEnd:	return SideIndex == 2;
	case ERoomShape::Spawn:		return SideIndex == 0;
	default:					return false;
	}
}

FBox ARoomBase::GetLocalBounds() const
{
	const float Half = CellSize * 0.5f;
	return FBox(FVector(-Half, -Half, 0.f), FVector(Half, Half, WallHeight));
}

void ARoomBase::BuildRoom()
{
	if (!RoomRoot)
	{
		return;
	}

	// Clear previously built geometry (editor reconstruction safety).
	TArray<USceneComponent*> Existing;
	RoomRoot->GetChildrenComponents(true, Existing);
	for (USceneComponent* C : Existing)
	{
		if (C)
		{
			C->DestroyComponent();
		}
	}
	Connectors.Empty();

	const float Half = CellSize * 0.5f;

	// Floor (top surface at z = 0) and ceiling.
	AddBox(FVector(0, 0, -WallThickness * 0.5f), FVector(CellSize, CellSize, WallThickness), 0.f);
	AddBox(FVector(0, 0, WallHeight + WallThickness * 0.5f), FVector(CellSize, CellSize, WallThickness), 0.f);

	for (int32 Side = 0; Side < 4; ++Side)
	{
		const bool bDoor = SideHasConnector(Side);
		AddSideWall(Side, bDoor);
		if (bDoor)
		{
			AddConnector(Side);
		}
	}
}

void ARoomBase::AddSideWall(int32 SideIndex, bool bHasDoor)
{
	const float Half = CellSize * 0.5f;
	const float Yaw = SideYaw(SideIndex);

	// Build in a frame where the wall faces +X, then rotate by Yaw.
	if (!bHasDoor)
	{
		AddBox(FVector(Half, 0, WallHeight * 0.5f), FVector(WallThickness, CellSize, WallHeight), Yaw);
		return;
	}

	const float SegLen = (CellSize - DoorWidth) * 0.5f;
	const float SegCenterY = DoorWidth * 0.5f + SegLen * 0.5f;

	// Left and right wall segments beside the doorway.
	AddBox(FVector(Half, -SegCenterY, WallHeight * 0.5f), FVector(WallThickness, SegLen, WallHeight), Yaw);
	AddBox(FVector(Half, SegCenterY, WallHeight * 0.5f), FVector(WallThickness, SegLen, WallHeight), Yaw);

	// Lintel above the doorway.
	const float LintelHeight = WallHeight - DoorHeight;
	if (LintelHeight > 1.f)
	{
		AddBox(FVector(Half, 0, DoorHeight + LintelHeight * 0.5f), FVector(WallThickness, DoorWidth, LintelHeight), Yaw);
	}
}

void ARoomBase::AddBox(const FVector& LocalPos, const FVector& SizeUU, float YawDeg)
{
	if (!CubeMesh)
	{
		return;
	}

	const FRotator Rot(0.f, YawDeg, 0.f);
	const FVector RotatedPos = Rot.RotateVector(LocalPos);

	UStaticMeshComponent* Comp = NewObject<UStaticMeshComponent>(this);
	Comp->SetStaticMesh(CubeMesh);
	Comp->SetupAttachment(RoomRoot);
	Comp->SetRelativeLocation(RotatedPos);
	Comp->SetRelativeRotation(Rot);
	Comp->SetRelativeScale3D(SizeUU / 100.f); // engine cube is 100uu
	Comp->SetCollisionProfileName(TEXT("BlockAll"));
	Comp->RegisterComponent();
}

void ARoomBase::AddConnector(int32 SideIndex)
{
	const float Half = CellSize * 0.5f;
	const FVector Dir = SideDir(SideIndex);

	URoomConnector* Conn = NewObject<URoomConnector>(this);
	Conn->SetupAttachment(RoomRoot);
	Conn->SetRelativeLocation(Dir * Half);
	Conn->SetRelativeRotation(FRotator(0.f, SideYaw(SideIndex), 0.f));
	Conn->RegisterComponent();
	Connectors.Add(Conn);
}
