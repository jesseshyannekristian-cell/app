#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "ContainmentTerminal.generated.h"

class UStaticMeshComponent;

/**
 * Interact with a sufficient keycard to re-contain the nearest breached SCP
 * within range. The core "way to contain them" loop.
 */
UCLASS()
class FOUNDATION_API AContainmentTerminal : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AContainmentTerminal();

	UPROPERTY(EditAnywhere, Category = "Containment")
	float ContainmentRange = 2500.f;

	UPROPERTY(VisibleAnywhere, Category = "Containment")
	UStaticMeshComponent* Mesh;

	virtual void Interact_Implementation(AFoundationCharacter* Interactor) override;
	virtual FString GetInteractPrompt_Implementation() const override;
};
