#pragma once

#include "CoreMinimal.h"
#include "SCPEntityBase.h"
#include "InteractableInterface.h"
#include "SCP16829.generated.h"

class UPointLightComponent;

/**
 * SCP-16829 — "The TV in Time" (Dimension Chemosic).
 * A stationary anomalous television. Not a pursuer; instead it warps space-time:
 *  - INTERACT (E): step into Dimension Chemosic — dimensional travel to a random
 *    room elsewhere in the facility (as O5 "GUNZ" is suspected to do between sites).
 *  - PASSIVE: standing within its field accrues temporal exposure and, after a delay,
 *    rewinds the player to where they entered the field (a time loop).
 *  - Prolonged exposure drains sanity and becomes carcinogenic (see Agent CHEMOSIC).
 */
UCLASS()
class FOUNDATION_API ASCP16829 : public ASCPEntityBase, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ASCP16829();

	virtual void Tick(float DeltaTime) override;

	// IInteractableInterface
	virtual void Interact_Implementation(AFoundationCharacter* Interactor) override;
	virtual FString GetInteractPrompt_Implementation() const override;

	UPROPERTY(EditAnywhere, Category = "SCP|16829")
	float LoopFieldRadius = 600.f;

	UPROPERTY(EditAnywhere, Category = "SCP|16829")
	float LoopInterval = 12.f;

	UPROPERTY(EditAnywhere, Category = "SCP|16829")
	float ProximitySanityDrain = 4.f;

	UPROPERTY(EditAnywhere, Category = "SCP|16829")
	float ExposurePerUse = 25.f;

	UPROPERTY(EditAnywhere, Category = "SCP|16829")
	float ExposureCancerThreshold = 75.f;

	UPROPERTY(EditAnywhere, Category = "SCP|16829")
	float CancerDamagePerSec = 3.f;

protected:
	UPROPERTY(VisibleAnywhere, Category = "SCP|16829")
	UPointLightComponent* ScreenGlow;

	float Exposure = 0.f;
	bool bPlayerInField = false;
	FVector LoopAnchor = FVector::ZeroVector;
	float LoopTimer = 0.f;

	bool FindRandomRoomLocation(FVector& OutLocation) const;
	void ApplyExposure(AFoundationCharacter* Player, float Amount);
};
