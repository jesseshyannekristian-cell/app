#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "DoorActor.generated.h"

class UStaticMeshComponent;

/** A keycard-gated sliding door. Opens if the interactor's keycard level is high enough. */
UCLASS()
class FOUNDATION_API ADoorActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ADoorActor();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	int32 RequiredKeycardLevel = 1;

	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenHeight = 320.f;

	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenSpeed = 400.f;

	UPROPERTY(VisibleAnywhere, Category = "Door")
	UStaticMeshComponent* DoorMesh;

	virtual void Interact_Implementation(AFoundationCharacter* Interactor) override;
	virtual FString GetInteractPrompt_Implementation() const override;

protected:
	bool bOpen = false;
	float ClosedZ = 0.f;
};
