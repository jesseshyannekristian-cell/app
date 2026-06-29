#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Styling/SlateTypes.h"
#include "Brushes/SlateColorBrush.h"

class UTexture2D;

/**
 * Title screen for "SCP SITE OVERSEER".
 * The supplied mockup art is shown as a letterboxed (aspect-preserving) backdrop, and
 * real interactive buttons are anchored on top of the painted START/OPTIONS/CREDITS.
 */
class SMainMenuWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMainMenuWidget) {}
		SLATE_ARGUMENT(UTexture2D*, BackgroundTexture)
		SLATE_EVENT(FSimpleDelegate, OnStartGame)
		SLATE_EVENT(FSimpleDelegate, OnQuit)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	enum class EPanel : uint8 { Main, Options, Credits };
	EPanel Current = EPanel::Main;

	FSimpleDelegate OnStartGame;
	FSimpleDelegate OnQuit;

	FSlateBrush BackgroundBrush;
	FSlateColorBrush DimBrush = FSlateColorBrush(FLinearColor(0.f, 0.f, 0.f, 0.55f));
	FButtonStyle ClearButtonStyle;

	FReply HandleStart();
	FReply HandleOptions();
	FReply HandleCredits();
	FReply HandleQuit();
	FReply HandleBack();

	EVisibility GetPanelVisibility(EPanel Panel) const;

	TSharedRef<SWidget> BuildButtonsOverlay();
	TSharedRef<SWidget> BuildOptionsPanel();
	TSharedRef<SWidget> BuildCreditsPanel();
};
