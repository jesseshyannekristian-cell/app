#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExitZone.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

/** Reaching this volume (with the required keycard) wins the run. */
UCLASS()
class FOUNDATION_API AExitZone : public AActor
{
	GENERATED_BODY()

public:
	AExitZone();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exit")
	int32 RequiredKeycardLevel = 3;

	UPROPERTY(VisibleAnywhere, Category = "Exit")
	UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Exit")
	UStaticMeshComponent* Marker;

protected:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep);
};
