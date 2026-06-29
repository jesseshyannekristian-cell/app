#include "SMainMenuWidget.h"
#include "Engine/Texture2D.h"
#include "Styling/CoreStyle.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SConstraintCanvas.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "MainMenu"

void SMainMenuWidget::Construct(const FArguments& InArgs)
{
	OnStartGame = InArgs._OnStartGame;
	OnQuit = InArgs._OnQuit;

	if (InArgs._BackgroundTexture)
	{
		BackgroundBrush.SetResourceObject(InArgs._BackgroundTexture);
		BackgroundBrush.ImageSize = FVector2D(InArgs._BackgroundTexture->GetSizeX(), InArgs._BackgroundTexture->GetSizeY());
		BackgroundBrush.DrawAs = ESlateBrushDrawType::Image;
	}
	else
	{
		// Fallback: solid near-black so the menu is still usable without the art file.
		BackgroundBrush = FSlateColorBrush(FLinearColor(0.02f, 0.03f, 0.04f, 1.f));
	}

	// Transparent button style with a subtle clearance-blue hover, layered over the painted buttons.
	ClearButtonStyle = FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button");
	ClearButtonStyle.SetNormal(FSlateColorBrush(FLinearColor(1.f, 1.f, 1.f, 0.f)));
	ClearButtonStyle.SetHovered(FSlateColorBrush(FLinearColor(0.30f, 0.72f, 1.f, 0.20f)));
	ClearButtonStyle.SetPressed(FSlateColorBrush(FLinearColor(0.30f, 0.72f, 1.f, 0.32f)));

	ChildSlot
	[
		SNew(SOverlay)

		// Layer 0 — full-screen black behind the letterboxed art.
		+ SOverlay::Slot()
		[
			SNew(SImage).Image(&DimBrush)
		]

		// Layer 1 — the title art + interactive buttons (aspect preserved, never cropped).
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SScaleBox)
			.Stretch(EStretch::ScaleToFit)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(1000.f)
				.HeightOverride(1000.f)
				[
					BuildButtonsOverlay()
				]
			]
		]

		// Layer 2 — Options panel.
		+ SOverlay::Slot()
		[
			SNew(SBox)
			.Visibility(this, &SMainMenuWidget::GetPanelVisibility, EPanel::Options)
			[
				BuildOptionsPanel()
			]
		]

		// Layer 3 — Credits panel.
		+ SOverlay::Slot()
		[
			SNew(SBox)
			.Visibility(this, &SMainMenuWidget::GetPanelVisibility, EPanel::Credits)
			[
				BuildCreditsPanel()
			]
		]
	];
}

TSharedRef<SWidget> SMainMenuWidget::BuildButtonsOverlay()
{
	return SNew(SConstraintCanvas)

		// Background art fills the 1000x1000 frame.
		+ SConstraintCanvas::Slot()
		.Anchors(FAnchors(0.f, 0.f, 1.f, 1.f))
		.Offset(FMargin(0.f))
		.Alignment(FVector2D(0.f, 0.f))
		[
			SNew(SImage).Image(&BackgroundBrush)
		]

		// START GAME (over the painted button).
		+ SConstraintCanvas::Slot()
		.Anchors(FAnchors(0.285f, 0.712f, 0.715f, 0.762f))
		.Offset(FMargin(0.f))
		.Alignment(FVector2D(0.f, 0.f))
		[
			SNew(SButton)
			.ButtonStyle(&ClearButtonStyle)
			.OnClicked(this, &SMainMenuWidget::HandleStart)
		]

		// OPTIONS.
		+ SConstraintCanvas::Slot()
		.Anchors(FAnchors(0.285f, 0.793f, 0.715f, 0.843f))
		.Offset(FMargin(0.f))
		.Alignment(FVector2D(0.f, 0.f))
		[
			SNew(SButton)
			.ButtonStyle(&ClearButtonStyle)
			.OnClicked(this, &SMainMenuWidget::HandleOptions)
		]

		// CREDITS.
		+ SConstraintCanvas::Slot()
		.Anchors(FAnchors(0.285f, 0.860f, 0.715f, 0.910f))
		.Offset(FMargin(0.f))
		.Alignment(FVector2D(0.f, 0.f))
		[
			SNew(SButton)
			.ButtonStyle(&ClearButtonStyle)
			.OnClicked(this, &SMainMenuWidget::HandleCredits)
		];
}

TSharedRef<SWidget> SMainMenuWidget::BuildOptionsPanel()
{
	const FSlateColor TitleColor(FLinearColor(0.30f, 0.72f, 1.f));
	const FSlateColor BodyColor(FLinearColor(0.80f, 0.85f, 0.90f));

	return SNew(SImage)
		.Image(&DimBrush)
		+ SNew(SOverlay);
}

TSharedRef<SWidget> SMainMenuWidget::BuildCreditsPanel()
{
	return SNew(SImage).Image(&DimBrush);
}

EVisibility SMainMenuWidget::GetPanelVisibility(EPanel Panel) const
{
	return (Current == Panel) ? EVisibility::Visible : EVisibility::Collapsed;
}

FReply SMainMenuWidget::HandleStart()
{
	OnStartGame.ExecuteIfBound();
	return FReply::Handled();
}

FReply SMainMenuWidget::HandleOptions()
{
	Current = EPanel::Options;
	return FReply::Handled();
}

FReply SMainMenuWidget::HandleCredits()
{
	Current = EPanel::Credits;
	return FReply::Handled();
}

FReply SMainMenuWidget::HandleQuit()
{
	OnQuit.ExecuteIfBound();
	return FReply::Handled();
}

FReply SMainMenuWidget::HandleBack()
{
	Current = EPanel::Main;
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
