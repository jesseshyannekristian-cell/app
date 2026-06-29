#include "SanityComponent.h"

USanityComponent::USanityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USanityComponent::BeginPlay()
{
	Super::BeginPlay();
	ScheduleNextBlink();
}

void USanityComponent::ScheduleNextBlink()
{
	// Lower sanity -> shorter interval (blink more often).
	const float SanityFactor = FMath::Clamp(Sanity / 100.f, 0.15f, 1.f);
	TimeToNextBlink = FMath::FRandRange(MinBlinkInterval, MaxBlinkInterval) * SanityFactor;
}

void USanityComponent::ForceBlink()
{
	bIsBlinking = true;
	BlinkRemaining = BlinkDuration;
}

void USanityComponent::DrainSanity(float Amount)
{
	Sanity = FMath::Clamp(Sanity - Amount, 0.f, 100.f);
}

void USanityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsBlinking)
	{
		BlinkRemaining -= DeltaTime;
		if (BlinkRemaining <= 0.f)
		{
			bIsBlinking = false;
			ScheduleNextBlink();
		}
		return;
	}

	TimeToNextBlink -= DeltaTime;
	if (TimeToNextBlink <= 0.f)
	{
		ForceBlink();
	}
}
