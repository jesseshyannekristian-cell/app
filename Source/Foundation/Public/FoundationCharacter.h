#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Faction.h"
#include "FoundationCharacter.generated.h"

class UCameraComponent;
class USpotLightComponent;
class USanityComponent;
class UInputComponent;

UCLASS()
class FOUNDATION_API AFoundationCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFoundationCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = "Player")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = "Player")
	USpotLightComponent* Flashlight;

	UPROPERTY(VisibleAnywhere, Category = "Player")
	USanityComponent* Sanity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	int32 KeycardLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Player")
	float InteractDistance = 260.f;

	UPROPERTY(EditAnywhere, Category = "Player")
	float WalkSpeed = 320.f;

	UPROPERTY(EditAnywhere, Category = "Player")
	float SprintSpeed = 620.f;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void GiveKeycard(int32 Level);

	UFUNCTION(BlueprintCallable, Category = "Player")
	bool IsBlinking() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void KillPlayer();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category = "Player")
	EFaction GetFaction() const { return EFaction::Foundation; }

protected:
	bool bDead = false;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void DoInteract();
	void ToggleFlashlight();
	void StartSprint();
	void StopSprint();
};
