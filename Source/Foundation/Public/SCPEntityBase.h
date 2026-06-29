#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SCPEntityBase.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;
class AFoundationCharacter;

UENUM(BlueprintType)
enum class EContainmentClass : uint8
{
	Safe,
	Euclid,
	Keter,
	Thaumiel
};

/**
 * Base hostile anomaly. Walks toward the player and kills on contact.
 * Subclasses override CanMove() to implement containment behaviours
 * (e.g. SCP-173 freezing while observed).
 */
UCLASS()
class FOUNDATION_API ASCPEntityBase : public APawn
{
	GENERATED_BODY()

public:
	ASCPEntityBase();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "SCP")
	UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, Category = "SCP")
	UStaticMeshComponent* BodyMesh;

	UPROPERTY(EditAnywhere, Category = "SCP")
	float MoveSpeed = 240.f;

	UPROPERTY(EditAnywhere, Category = "SCP")
	float KillDistance = 130.f;

	UPROPERTY(EditAnywhere, Category = "SCP")
	FString Designation = TEXT("SCP-939");

	UPROPERTY(EditAnywhere, Category = "SCP|Containment")
	EContainmentClass ContainmentClass = EContainmentClass::Euclid;

	UPROPERTY(EditAnywhere, Category = "SCP|Containment", meta = (MultiLine = true))
	FString ContainmentProcedure = TEXT("Lure into a standard humanoid containment cell and seal the blast door.");

	/** Keycard level required at a Containment Terminal to re-contain this entity. */
	UPROPERTY(EditAnywhere, Category = "SCP|Containment")
	int32 ContainmentKeycardLevel = 2;

	UPROPERTY(BlueprintReadOnly, Category = "SCP|Containment")
	bool bContained = false;

	UPROPERTY(EditAnywhere, Category = "SCP")
	float Health = 600.f;

	/** Re-contains this anomaly (disables it and removes the threat). */
	UFUNCTION(BlueprintCallable, Category = "SCP|Containment")
	virtual void Recontain();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	AFoundationCharacter* GetTargetPlayer() const;
	virtual bool CanMove(AFoundationCharacter* Player) const;
	void MoveTowardPlayer(float DeltaTime, AFoundationCharacter* Player);
	void TryKill(AFoundationCharacter* Player);
};
