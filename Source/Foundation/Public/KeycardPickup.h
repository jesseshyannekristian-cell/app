#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "KeycardPickup.generated.h"

class UStaticMeshComponent;

UCLASS()
class FOUNDATION_API AKeycardPickup : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AKeycardPickup();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keycard")
	int32 KeycardLevel = 1;

	UPROPERTY(VisibleAnywhere, Category = "Keycard")
	UStaticMeshComponent* Mesh;

	virtual void Interact_Implementation(AFoundationCharacter* Interactor) override;
	virtual FString GetInteractPrompt_Implementation() const override;
};
