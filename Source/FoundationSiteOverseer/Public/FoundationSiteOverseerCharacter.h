#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FoundationSiteOverseerCharacter.generated.h"

class UCameraComponent;

UCLASS()
class FOUNDATIONSITEOVERSEER_API AFoundationSiteOverseerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFoundationSiteOverseerCharacter();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = "0.0"))
	float WeaponDamage = 25.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = "0.0"))
	float WeaponRange = 10000.0f;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Fire();
};