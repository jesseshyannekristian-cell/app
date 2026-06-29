#pragma once

#include "CoreMinimal.h"
#include "SCPEntityBase.h"
#include "SCP096.generated.h"

/**
 * SCP-096 — "The Shy Guy". Utterly passive until a person views its face.
 * Once viewed it becomes enraged, sprinting toward the viewer until they
 * are eliminated. Opposite mechanic to SCP-173.
 */
UCLASS()
class FOUNDATION_API ASCP096 : public ASCPEntityBase
{
	GENERATED_BODY()

public:
	ASCP096();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "SCP|096")
	bool bEnraged = false;

	UPROPERTY(EditAnywhere, Category = "SCP|096")
	float ViewDotThreshold = 0.7f;

protected:
	virtual bool CanMove(AFoundationCharacter* Player) const override;
	void DetectViewed();
};
