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
			.Visibility_Lambda([this]() { return GetPanelVisibility(EPanel::Options); })
			[
				BuildOptionsPanel()
			]
		]

		// Layer 3 — Credits panel.
		+ SOverlay::Slot()
		[
			SNew(SBox)
			.Visibility_Lambda([this]() { return GetPanelVisibility(EPanel::Credits); })
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
	static FSlateColorBrush PanelBg(FLinearColor(0.02f, 0.03f, 0.04f, 0.94f));
	const FSlateColor TitleColor(FLinearColor(0.30f, 0.72f, 1.f));
	const FSlateColor BodyColor(FLinearColor(0.80f, 0.85f, 0.90f));

	return SNew(SOverlay)
		+ SOverlay::Slot()[ SNew(SImage).Image(&PanelBg) ]
		+ SOverlay::Slot()
		.HAlign(HAlign_Center).VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 0, 0, 24)
			[ SNew(STextBlock).Font(FCoreStyle::GetDefaultFontStyle("Bold", 40)).ColorAndOpacity(TitleColor).Text(LOCTEXT("Options", "OPTIONS")) ]
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 0, 0, 28)
			[
				SNew(STextBlock).Font(FCoreStyle::GetDefaultFontStyle("Regular", 16)).ColorAndOpacity(BodyColor).Justification(ETextJustify::Center)
				.Text(LOCTEXT("OptionsBody", "Display, audio and control settings are configured in-engine via\nProject Settings and the Settings menu. Press ESC in-game to pause."))
			]
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 0, 0, 12)
			[
				SNew(SBox).MinDesiredWidth(280.f)
				[ SNew(SButton).HAlign(HAlign_Center).OnClicked(this, &SMainMenuWidget::HandleQuit)
					[ SNew(STextBlock).Font(FCoreStyle::GetDefaultFontStyle("Bold", 18)).Text(LOCTEXT("Quit", "QUIT GAME")) ] ]
			]
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[
				SNew(SBox).MinDesiredWidth(280.f)
				[ SNew(SButton).HAlign(HAlign_Center).OnClicked(this, &SMainMenuWidget::HandleBack)
					[ SNew(STextBlock).Font(FCoreStyle::GetDefaultFontStyle("Bold", 18)).Text(LOCTEXT("BackBtn", "BACK")) ] ]
			]
		];
}

TSharedRef<SWidget> SMainMenuWidget::BuildCreditsPanel()
{
	static FSlateColorBrush PanelBg(FLinearColor(0.02f, 0.03f, 0.04f, 0.94f));
	const FSlateColor TitleColor(FLinearColor(0.30f, 0.72f, 1.f));
	const FSlateColor BodyColor(FLinearColor(0.80f, 0.85f, 0.90f));

	const FText CreditsText = LOCTEXT("CreditsBody",
		"SCP SITE OVERSEER\n\n"
		"A non-official, fan-made game built in Unreal Engine 5.5.\n\n"
		"SCP FOUNDATION CONTENT\n"
		"All SCP names, numbers, lore and Groups of Interest are the creative work of the\n"
		"SCP Foundation community, licensed under Creative Commons Attribution-ShareAlike 3.0\n"
		"(CC BY-SA 3.0). https://scp-wiki.wikidot.com/  \u2022  https://creativecommons.org/licenses/by-sa/3.0/\n\n"
		"This project's source code and content are likewise released under CC BY-SA 3.0.\n\n"
		"ENGINE\n"
		"Unreal\u00AE Engine \u00A9 Epic Games, Inc. \u2014 governed by the Unreal Engine EULA.\n\n"
		"Not affiliated with or endorsed by the SCP Foundation or Epic Games.");

	return SNew(SOverlay)
		+ SOverlay::Slot()[ SNew(SImage).Image(&PanelBg) ]
		+ SOverlay::Slot()
		.HAlign(HAlign_Center).VAlign(VAlign_Center).Padding(60.f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 0, 0, 20)
			[ SNew(STextBlock).Font(FCoreStyle::GetDefaultFontStyle("Bold", 40)).ColorAndOpacity(TitleColor).Text(LOCTEXT("Credits", "CREDITS")) ]
			+ SVerticalBox::Slot().FillHeight(1.f).Padding(0, 0, 0, 20)
			[
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				[ SNew(STextBlock).Font(FCoreStyle::GetDefaultFontStyle("Regular", 15)).ColorAndOpacity(BodyColor).Justification(ETextJustify::Center).AutoWrapText(true).Text(CreditsText) ]
			]
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
			[
				SNew(SBox).MinDesiredWidth(280.f)
				[ SNew(SButton).HAlign(HAlign_Center).OnClicked(this, &SMainMenuWidget::HandleBack)
					[ SNew(STextBlock).Font(FCoreStyle::GetDefaultFontStyle("Bold", 18)).Text(LOCTEXT("BackBtn2", "BACK")) ] ]
			]
		];
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
