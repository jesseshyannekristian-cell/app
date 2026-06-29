#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomBase.h"
#include "FacilityGenerator.generated.h"

class ARoomBase;
class ASCPEntityBase;
class AExitZone;
class AKeycardPickup;
class AContainmentTerminal;
class UStaticMesh;

USTRUCT()
struct FOpenConnector
{
	GENERATED_BODY()

	UPROPERTY()
	ARoomBase* Room = nullptr;

	int32 ConnectorIndex = 0;
	FTransform WorldXform;
};

/**
 * Procedurally stitches modular rooms together connector-to-connector,
 * caps dangling openings, then populates SCP entities, keycards and the
 * escape zone. Drop one into your level (or let the GameMode spawn it).
 */
UCLASS()
class FOUNDATION_API AFacilityGenerator : public AActor
{
	GENERATED_BODY()

public:
	AFacilityGenerator();

	UPROPERTY(EditAnywhere, Category = "Generation")
	int32 TargetRoomCount = 40;

	UPROPERTY(EditAnywhere, Category = "Generation")
	int32 RandomSeed = 0;

	UPROPERTY(EditAnywhere, Category = "Generation|Spawning")
	TArray<TSubclassOf<ASCPEntityBase>> SCPClasses;

	UPROPERTY(EditAnywhere, Category = "Generation|Spawning")
	int32 SCPCount = 3;

	UPROPERTY(EditAnywhere, Category = "Generation|Spawning")
	int32 KeycardCount = 4;

	UPROPERTY(EditAnywhere, Category = "Generation|Spawning")
	TSubclassOf<AExitZone> ExitZoneClass;

	UPROPERTY(EditAnywhere, Category = "Generation|Spawning")
	TSubclassOf<AKeycardPickup> KeycardClass;

	UPROPERTY(EditAnywhere, Category = "Generation|Spawning")
	TSubclassOf<AContainmentTerminal> TerminalClass;

	UPROPERTY(EditAnywhere, Category = "Generation|Spawning")
	int32 TerminalCount = 5;

	/** Guaranteed signature anomaly placed once per facility: SCP-16829 "The TV in Time". */
	UPROPERTY(EditAnywhere, Category = "Generation|Spawning")
	TSubclassOf<ASCPEntityBase> SignatureAnomalyClass;

	UPROPERTY()
	TArray<ARoomBase*> PlacedRooms;

	UFUNCTION(BlueprintCallable, Category = "Generation")
	void Generate();

	FVector GetStartLocation() const;

protected:
	FRandomStream Rng;
	TArray<FBox> PlacedBounds;

	UPROPERTY()
	UStaticMesh* CubeMesh = nullptr;

	ARoomBase* SpawnShapedRoom(ERoomShape ShapeToUse, const FTransform& Xform);
	void RegisterRoom(ARoomBase* Room, TArray<FOpenConnector>& OpenList);
	bool TryPlaceRoom(const FOpenConnector& Target, TArray<FOpenConnector>& OutNewOpen);
	bool OverlapsExisting(const FBox& Box) const;
	FBox ComputeWorldBounds(ARoomBase* Room, const FTransform& Xform) const;
	void CapConnector(const FOpenConnector& Open);
	void PopulateFacility();
	ERoomShape RandomShape();
};
