#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomBase.generated.h"

class URoomConnector;
class UStaticMesh;

UENUM(BlueprintType)
enum class ERoomShape : uint8
{
	Straight,	// connectors on +X and -X
	Corner,		// connectors on +X and +Y
	TJunction,	// connectors on +X, +Y, -Y
	Cross,		// connectors on all four sides
	DeadEnd,	// single connector on -X
	Spawn		// single connector on +X (start room)
};

/**
 * A modular facility room. Geometry (floor / ceiling / walls with door gaps)
 * is built procedurally from the engine cube mesh, so the project is playable
 * with zero art assets. Replace the meshes with your own modular kit later.
 */
UCLASS()
class FOUNDATION_API ARoomBase : public AActor
{
	GENERATED_BODY()

public:
	ARoomBase();

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	ERoomShape Shape = ERoomShape::Straight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	float CellSize = 1000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
	TArray<URoomConnector*> Connectors;

	/** Local-space bounding box used by the generator for overlap tests. */
	FBox GetLocalBounds() const;

protected:
	UPROPERTY()
	USceneComponent* RoomRoot = nullptr;

	UPROPERTY()
	UStaticMesh* CubeMesh = nullptr;

	void BuildRoom();
	bool SideHasConnector(int32 SideIndex) const;
	void AddBox(const FVector& LocalPos, const FVector& SizeUU, float YawDeg);
	void AddSideWall(int32 SideIndex, bool bHasDoor);
	void AddConnector(int32 SideIndex);

	static FVector SideDir(int32 SideIndex);
	static float SideYaw(int32 SideIndex);
};
