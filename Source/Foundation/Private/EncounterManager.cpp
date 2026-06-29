#include "EncounterManager.h"
#include "FactionCharacter.h"
#include "SCPEntityBase.h"
#include "SCP173.h"
#include "SCP096.h"
#include "SCP106.h"
#include "SCP682.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

AEncounterManager::AEncounterManager()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	FactionCharacterClass = AFactionCharacter::StaticClass();
}

void AEncounterManager::BeginPlay()
{
	Super::BeginPlay();

	if (SCPPool.Num() == 0)
	{
		SCPPool.Add(ASCP173::StaticClass());
		SCPPool.Add(ASCP096::StaticClass());
		SCPPool.Add(ASCP106::StaticClass());
		SCPPool.Add(ASCP682::StaticClass());
	}

	SpawnXI87Squad();
}

void AEncounterManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Timer += DeltaTime;
	if (Timer >= EncounterInterval)
	{
		Timer = 0.f;
		if (CountActiveNPCs() < MaxActiveNPCs)
		{
			TriggerRandomEncounter();
		}
	}
}

int32 AEncounterManager::CountActiveNPCs() const
{
	int32 Count = 0;
	for (TActorIterator<AFactionCharacter> It(GetWorld()); It; ++It) { ++Count; }
	for (TActorIterator<ASCPEntityBase> It(GetWorld()); It; ++It)
	{
		if (!It->bContained) { ++Count; }
	}
	return Count;
}

bool AEncounterManager::FindSpawnPoint(const FVector& Around, FVector& OutLoc) const
{
	for (int32 i = 0; i < 8; ++i)
	{
		const float Angle = FMath::FRandRange(0.f, 2.f * PI);
		const float Dist = FMath::FRandRange(MinSpawnDist, SpawnRadius);
		const FVector Candidate = Around + FVector(FMath::Cos(Angle) * Dist, FMath::Sin(Angle) * Dist, 300.f);

		FHitResult Hit;
		const FVector End = Candidate - FVector(0, 0, 1500.f);
		if (GetWorld()->LineTraceSingleByChannel(Hit, Candidate, End, ECC_Visibility))
		{
			OutLoc = Hit.ImpactPoint + FVector(0, 0, 100.f);
			return true;
		}
	}
	return false;
}

void AEncounterManager::TriggerRandomEncounter()
{
	const int32 Roll = FMath::RandRange(0, 5);
	switch (Roll)
	{
	case 0: SpawnPatrol(EFaction::MTF, FMath::RandRange(1, 2), TEXT("MTF Trooper")); break;
	case 1: SpawnPatrol(EFaction::ChaosInsurgency, FMath::RandRange(2, 3), TEXT("Insurgent")); break;
	case 2: SpawnPatrol(EFaction::UIU, 2, TEXT("UIU Agent")); break;
	case 3: SpawnPatrol(EFaction::Wondertainment, 1, TEXT("Wondertainment Rep")); break;
	case 4: SpawnPatrol(EFaction::Civilian, FMath::RandRange(1, 2), TEXT("Survivor")); break;
	default: SpawnSCPBreach(); break;
	}
}

void AEncounterManager::SpawnPatrol(EFaction Faction, int32 Count, const FString& NamePrefix)
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Player) { return; }

	FVector Base;
	if (!FindSpawnPoint(Player->GetActorLocation(), Base)) { return; }

	for (int32 i = 0; i < Count; ++i)
	{
		const FVector Loc = Base + FVector(FMath::FRandRange(-200.f, 200.f), FMath::FRandRange(-200.f, 200.f), 0.f);
		SpawnOperative(Faction, Loc, FString::Printf(TEXT("%s %d"), *NamePrefix, i + 1), TEXT(""), TEXT("Agent"));
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Silver,
			FString::Printf(TEXT("Encounter: %s detected nearby."), *UFactionLibrary::GetFactionName(Faction)));
	}
}

void AEncounterManager::SpawnSCPBreach()
{
	if (SCPPool.Num() == 0) { return; }
	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Player) { return; }

	FVector Loc;
	if (!FindSpawnPoint(Player->GetActorLocation(), Loc)) { return; }

	TSubclassOf<ASCPEntityBase> Cls = SCPPool[FMath::RandRange(0, SCPPool.Num() - 1)];
	if (Cls)
	{
		GetWorld()->SpawnActor<ASCPEntityBase>(Cls, Loc, FRotator::ZeroRotator);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("WARNING: Containment breach detected!"));
		}
	}
}

AFactionCharacter* AEncounterManager::SpawnOperative(EFaction Faction, const FVector& Loc, const FString& Name, const FString& Callsign, const FString& Rank)
{
	if (!FactionCharacterClass) { return nullptr; }

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AFactionCharacter* NPC = GetWorld()->SpawnActor<AFactionCharacter>(FactionCharacterClass, Loc, FRotator::ZeroRotator, Params);
	if (NPC)
	{
		NPC->Faction = Faction;
		NPC->OperativeName = Name;
		NPC->Callsign = Callsign;
		NPC->Rank = Rank;
	}
	return NPC;
}

void AEncounterManager::SpawnXI87Squad()
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	const FVector Origin = Player ? Player->GetActorLocation() : GetActorLocation();

	FVector Base;
	if (!FindSpawnPoint(Origin, Base))
	{
		Base = Origin + FVector(400.f, 0.f, 0.f);
	}

	// MTF XI-87 "Binge Watchers" — roster from the dossier (allied to the player).
	SpawnOperative(EFaction::MTF, Base + FVector(0, 0, 0), TEXT("Marcus Kane"), TEXT("Loopbreaker"), TEXT("Lead Agent"));
	SpawnOperative(EFaction::MTF, Base + FVector(150, 150, 0), TEXT("Elena Voss"), TEXT(""), TEXT("Deputy"));
	SpawnOperative(EFaction::MTF, Base + FVector(150, -150, 0), TEXT("Riley Torres"), TEXT("Static"), TEXT("Agent"));
	SpawnOperative(EFaction::MTF, Base + FVector(300, 100, 0), TEXT("Jamal Reyes"), TEXT(""), TEXT("Agent"));
	SpawnOperative(EFaction::MTF, Base + FVector(300, -100, 0), TEXT("Sophia Laurent"), TEXT(""), TEXT("Agent"));

	if (AFactionCharacter* Gunz = SpawnOperative(EFaction::MTF, Base + FVector(450, 0, 0), TEXT("Kris"), TEXT("Gunz"), TEXT("O5-00 Attached")))
	{
		Gunz->Health = 220.f;
		Gunz->Damage = 28.f;
		Gunz->FireInterval = 0.7f;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Cyan, TEXT("MTF XI-87 'Binge Watchers' deployed alongside you."));
	}
}
