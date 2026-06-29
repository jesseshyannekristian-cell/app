#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FoundationHUD.generated.h"

class SFoundationHUDWidget;

/** In-breach HUD: shows health, sanity/blink, keycard clearance and the objective. */
UCLASS()
class FOUNDATION_API AFoundationHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	TSharedPtr<SFoundationHUDWidget> HUDWidget;
};
