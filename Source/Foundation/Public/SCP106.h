#pragma once

#include "CoreMinimal.h"
#include "SCPEntityBase.h"
#include "SCP106.generated.h"

/**
 * SCP-106 — "The Old Man". Slow but inexorable; periodically phases/teleports
 * closer to its prey when not directly observed.
 */
UCLASS()
class FOUNDATION_API ASCP106 : public ASCPEntityBase
{
	GENERATED_BODY()

public:
	ASCP106();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "SCP|106")
	float TeleportInterval = 8.f;

	UPROPERTY(EditAnywhere, Category = "SCP|106")
	float TeleportMinRange = 1500.f;

protected:
	float TeleportTimer = 0.f;
	void TryPhase();
};
