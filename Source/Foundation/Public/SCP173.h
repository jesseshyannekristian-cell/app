#pragma once

#include "CoreMinimal.h"
#include "SCPEntityBase.h"
#include "SCP173.generated.h"

/**
 * SCP-173 — "The Sculpture". Cannot move while within a player's view and
 * direct line of sight. The instant the player blinks or breaks line of
 * sight, it advances rapidly and snaps the player's neck.
 */
UCLASS()
class FOUNDATION_API ASCP173 : public ASCPEntityBase
{
	GENERATED_BODY()

public:
	ASCP173();

	UPROPERTY(EditAnywhere, Category = "SCP|173")
	float ViewHalfAngleCos = 0.5f; // ~60 degrees half-FOV

protected:
	virtual bool CanMove(AFoundationCharacter* Player) const override;
	bool IsObservedBy(AFoundationCharacter* Player) const;
};
