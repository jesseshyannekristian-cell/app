#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SanityComponent.generated.h"

/**
 * Drives the involuntary "blink" cycle (when the player's eyes close,
 * line-of-sight SCPs like 173 are free to move) and a sanity value that
 * makes blinks more frequent as it drops.
 */
UCLASS(ClassGroup = (Foundation), meta = (BlueprintSpawnableComponent))
class FOUNDATION_API USanityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USanityComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity")
	float Sanity = 100.f;

	UPROPERTY(EditAnywhere, Category = "Sanity")
	float MinBlinkInterval = 4.f;

	UPROPERTY(EditAnywhere, Category = "Sanity")
	float MaxBlinkInterval = 7.f;

	UPROPERTY(EditAnywhere, Category = "Sanity")
	float BlinkDuration = 0.4f;

	UPROPERTY(BlueprintReadOnly, Category = "Sanity")
	bool bIsBlinking = false;

	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void ForceBlink();

	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void DrainSanity(float Amount);

protected:
	float TimeToNextBlink = 0.f;
	float BlinkRemaining = 0.f;

	void ScheduleNextBlink();
};
