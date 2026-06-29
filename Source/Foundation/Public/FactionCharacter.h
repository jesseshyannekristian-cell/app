#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Faction.h"
#include "FactionCharacter.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;

/**
 * A faction NPC (MTF, Chaos Insurgency, UIU, Wondertainment, Civilian...).
 * Roams, senses nearby actors, and engages anyone its faction is hostile
 * toward (the player, rival factions, or anomalies) with ranged fire.
 */
UCLASS()
class FOUNDATION_API AFactionCharacter : public APawn
{
	GENERATED_BODY()

public:
	AFactionCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	EFaction Faction = EFaction::MTF;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	FString OperativeName = TEXT("Operative");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	FString Callsign;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	FString Rank = TEXT("Agent");

	UPROPERTY(EditAnywhere, Category = "Combat")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float SenseRadius = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRange = 2000.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float MoveSpeed = 340.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireInterval = 1.1f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float Damage = 16.f;

	UPROPERTY(VisibleAnywhere, Category = "Faction")
	UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, Category = "Faction")
	UStaticMeshComponent* BodyMesh;

protected:
	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	float FireCooldown = 0.f;
	float RetargetCooldown = 0.f;
	float RoamCooldown = 0.f;
	FVector RoamDir = FVector::ForwardVector;

	void ApplyTint();
	EFaction GetActorFaction(AActor* Actor) const;
	AActor* FindHostileTarget();
	void EngageTarget(float DeltaTime);
	void Roam(float DeltaTime);
	bool HasLineOfSight(AActor* Target) const;
	void FireAt(AActor* Target);
};
