#include "SFoundationHUDWidget.h"
#include "FoundationHUD.h"
#include "FoundationCharacter.h"
#include "SanityComponent.h"
#include "Styling/CoreStyle.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Notifications/SProgressBar.h"

#define LOCTEXT_NAMESPACE "FoundationHUD"

namespace HUDColors
{
	static const FLinearColor Health(0.85f, 0.25f, 0.25f);
	static const FLinearColor Sanity(0.30f, 0.72f, 1.f);
	static const FLinearColor Gold(1.f, 0.78f, 0.25f);
	static const FLinearColor Gray(0.80f, 0.85f, 0.90f);
	static const FLinearColor Red(0.95f, 0.30f, 0.30f);
}

static FSlateFontInfo HUDFont(int32 Size, const TCHAR* Style = TEXT("Bold"))
{
	return FCoreStyle::GetDefaultFontStyle(Style, Size);
}

void SFoundationHUDWidget::Construct(const FArguments& InArgs)
{
	HUD = InArgs._OwnerHUD;

	ChildSlot
	[
		SNew(SOverlay)

		// Objective — top center.
		+ SOverlay::Slot()
		.HAlign(HAlign_Center).VAlign(VAlign_Top).Padding(0, 18, 0, 0)
		[
			SNew(SBorder).BorderImage(&PanelBg).Padding(FMargin(16, 6))
			[
				SNew(STextBlock).Font(HUDFont(13)).ColorAndOpacity(HUDColors::Gold)
				.Text(LOCTEXT("Objective", "OBJECTIVE: Find keycards \u2022 Re-contain breached SCPs \u2022 Reach the Exit Zone"))
			]
		]

		// Vitals — bottom left.
		+ SOverlay::Slot()
		.HAlign(HAlign_Left).VAlign(VAlign_Bottom).Padding(28, 0, 0, 28)
		[
			SNew(SBorder).BorderImage(&PanelBg).Padding(FMargin(16, 12))
			[
				SNew(SBox).WidthOverride(320.f)
				[
					SNew(SVerticalBox)

					// Health
					+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 2)
					[ SNew(STextBlock).Font(HUDFont(11)).ColorAndOpacity(HUDColors::Health).Text(LOCTEXT("HP", "HEALTH")) ]
					+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 8)
					[
						SNew(SProgressBar)
						.FillColorAndOpacity(HUDColors::Health)
						.Percent_Lambda([this]()
						{
							AFoundationCharacter* P = GetPlayer();
							return P ? FMath::Clamp(P->Health / 100.f, 0.f, 1.f) : 0.f;
						})
					]

					// Sanity
					+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 2)
					[
						SNew(STextBlock).Font(HUDFont(11))
						.ColorAndOpacity_Lambda([this]()
						{
							AFoundationCharacter* P = GetPlayer();
							const bool bBlink = P && P->IsBlinking();
							return FSlateColor(bBlink ? HUDColors::Red : HUDColors::Sanity);
						})
						.Text_Lambda([this]()
						{
							AFoundationCharacter* P = GetPlayer();
							return (P && P->IsBlinking()) ? LOCTEXT("SanityBlink", "SANITY  \u2014  *** BLINKING ***") : LOCTEXT("Sanity", "SANITY");
						})
					]
					+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 10)
					[
						SNew(SProgressBar)
						.FillColorAndOpacity(HUDColors::Sanity)
						.Percent_Lambda([this]()
						{
							AFoundationCharacter* P = GetPlayer();
							return (P && P->Sanity) ? FMath::Clamp(P->Sanity->Sanity / 100.f, 0.f, 1.f) : 0.f;
						})
					]

					// Keycard clearance
					+ SVerticalBox::Slot().AutoHeight()
					[
						SNew(STextBlock).Font(HUDFont(13)).ColorAndOpacity(HUDColors::Gray)
						.Text_Lambda([this]()
						{
							AFoundationCharacter* P = GetPlayer();
							const int32 Lvl = P ? P->KeycardLevel : 0;
							return FText::FromString(FString::Printf(TEXT("KEYCARD CLEARANCE: LEVEL %d"), Lvl));
						})
					]
				]
			]
		]
	];
}

AFoundationCharacter* SFoundationHUDWidget::GetPlayer() const
{
	if (HUD.IsValid())
	{
		return Cast<AFoundationCharacter>(HUD->GetOwningPawn());
	}
	return nullptr;
}

#undef LOCTEXT_NAMESPACE
