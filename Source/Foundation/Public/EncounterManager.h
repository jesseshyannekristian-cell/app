#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Faction.h"
#include "EncounterManager.generated.h"

class AFactionCharacter;
class ASCPEntityBase;

/**
 * Drives the open-world feel: continuously spawns random encounters around
 * the player (faction patrols, anomalous breaches, civilians) and seeds the
 * allied MTF XI-87 "Binge Watchers" squad at the start.
 */
UCLASS()
class FOUNDATION_API AEncounterManager : public AActor
{
	GENERATED_BODY()

public:
	AEncounterManager();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Encounter")
	float EncounterInterval = 22.f;

	UPROPERTY(EditAnywhere, Category = "Encounter")
	float SpawnRadius = 3500.f;

	UPROPERTY(EditAnywhere, Category = "Encounter")
	float MinSpawnDist = 1400.f;

	UPROPERTY(EditAnywhere, Category = "Encounter")
	int32 MaxActiveNPCs = 14;

	UPROPERTY(EditAnywhere, Category = "Encounter")
	TSubclassOf<AFactionCharacter> FactionCharacterClass;

	UPROPERTY(EditAnywhere, Category = "Encounter")
	TArray<TSubclassOf<ASCPEntityBase>> SCPPool;

protected:
	float Timer = 0.f;

	int32 CountActiveNPCs() const;
	bool FindSpawnPoint(const FVector& Around, FVector& OutLoc) const;
	void TriggerRandomEncounter();
	void SpawnPatrol(EFaction Faction, int32 Count, const FString& NamePrefix);
	void SpawnSCPBreach();
	void SpawnXI87Squad();
	AFactionCharacter* SpawnOperative(EFaction Faction, const FVector& Loc, const FString& Name, const FString& Callsign, const FString& Rank);
};
