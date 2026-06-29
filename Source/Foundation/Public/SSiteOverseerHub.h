#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Brushes/SlateColorBrush.h"

class UOverseerProgression;
class SBox;

/**
 * Site Overseer management hub. Tabs:
 * RESEARCH DIVISION | STORE | CONTAINMENT OPERATIONS | BREACH | RANKS & REWARDS
 */
class SSiteOverseerHub : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSiteOverseerHub) {}
		SLATE_ARGUMENT(UOverseerProgression*, Progression)
		SLATE_EVENT(FSimpleDelegate, OnDeployBreach)
		SLATE_EVENT(FSimpleDelegate, OnBackToTitle)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	enum class ETab : uint8 { Research, Store, Operations, Breach, Personnel, Ranks };
	ETab CurrentTab = ETab::Research;

	TWeakObjectPtr<UOverseerProgression> Progress;
	FSimpleDelegate OnDeployBreach;
	FSimpleDelegate OnBackToTitle;

	TSharedPtr<SBox> ContentBox;
	FString StatusMsg;

	FSlateColorBrush HubBg = FSlateColorBrush(FLinearColor(0.025f, 0.035f, 0.045f, 0.97f));
	FSlateColorBrush RowBg = FSlateColorBrush(FLinearColor(1.f, 1.f, 1.f, 0.04f));
	FSlateColorBrush BarBg = FSlateColorBrush(FLinearColor(0.04f, 0.06f, 0.08f, 1.f));

	void SetTab(ETab Tab);
	void RefreshContent();

	TSharedRef<SWidget> BuildTabBar();
	TSharedRef<SWidget> BuildTabButton(const FString& Label, ETab Tab);
	TSharedRef<SWidget> BuildHeader();
	TSharedRef<SWidget> BuildCurrentTabContent();

	TSharedRef<SWidget> BuildResearchTab();
	TSharedRef<SWidget> BuildStoreTab();
	TSharedRef<SWidget> BuildOperationsTab();
	TSharedRef<SWidget> BuildBreachTab();
	TSharedRef<SWidget> BuildPersonnelTab();
	TSharedRef<SWidget> BuildRanksTab();

	// Generic list row: title + subtitle (left), info (right), action button.
	TSharedRef<SWidget> MakeRow(const FString& Title, const FString& Subtitle, const FString& RightInfo,
		const FString& ButtonLabel, bool bEnabled, const FLinearColor& AccentColor, FOnClicked OnClick);

	FReply DoResearch(FName ProjectId);
	FReply DoPurchase(FName EquipId);
	FReply DoOperation(FName OpId);
	FReply DoDeployBreach();
	FReply DoBackToTitle();
};
