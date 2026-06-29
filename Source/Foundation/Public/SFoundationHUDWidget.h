#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Brushes/SlateColorBrush.h"

class AFoundationHUD;
class AFoundationCharacter;

/** Slate overlay HUD drawn during a live breach. */
class SFoundationHUDWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFoundationHUDWidget) {}
		SLATE_ARGUMENT(AFoundationHUD*, OwnerHUD)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TWeakObjectPtr<AFoundationHUD> HUD;
	FSlateColorBrush PanelBg = FSlateColorBrush(FLinearColor(0.f, 0.f, 0.f, 0.45f));

	AFoundationCharacter* GetPlayer() const;
};
