#include "SSiteOverseerHub.h"
#include "OverseerProgression.h"
#include "ArchiveSubsystem.h"
#include "FoundationDataLibrary.h"
#include "Styling/CoreStyle.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

#define LOCTEXT_NAMESPACE "OverseerHub"

namespace HubColors
{
	static const FLinearColor Cyan(0.30f, 0.72f, 1.f);
	static const FLinearColor Gold(1.f, 0.78f, 0.25f);
	static const FLinearColor Red(0.92f, 0.28f, 0.28f);
	static const FLinearColor Green(0.35f, 0.85f, 0.45f);
	static const FLinearColor Gray(0.70f, 0.75f, 0.80f);
	static const FLinearColor Dim(0.45f, 0.50f, 0.55f);
}

static FSlateFontInfo HubFont(int32 Size, const TCHAR* Style = TEXT("Regular"))
{
	return FCoreStyle::GetDefaultFontStyle(Style, Size);
}

void SSiteOverseerHub::Construct(const FArguments& InArgs)
{
	Progress = InArgs._Progression;
	Archive = InArgs._Archive;
	OnDeployBreach = InArgs._OnDeployBreach;
	OnBackToTitle = InArgs._OnBackToTitle;

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()[ SNew(SImage).Image(&HubBg) ]
		+ SOverlay::Slot()
		.Padding(FMargin(48.f, 28.f))
		[
			SNew(SVerticalBox)

			// Header (title + currencies + rank)
			+ SVerticalBox::Slot().AutoHeight()[ BuildHeader() ]

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 12)
			[ SNew(SSeparator).Orientation(Orient_Horizontal) ]

			// Tab bar
			+ SVerticalBox::Slot().AutoHeight()[ BuildTabBar() ]

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 10)
			[ SNew(SSeparator).Orientation(Orient_Horizontal) ]

			// Content
			+ SVerticalBox::Slot().FillHeight(1.f)
			[
				SAssignNew(ContentBox, SBox)[ BuildCurrentTabContent() ]
			]

			// Footer status line + back to title
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 12, 0, 0)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Font(HubFont(12))
					.ColorAndOpacity(HubColors::Green)
					.Text_Lambda([this]() { return FText::FromString(StatusMsg); })
				]
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SButton)
					.OnClicked(this, &SSiteOverseerHub::DoBackToTitle)
					[ SNew(STextBlock).Font(HubFont(13, TEXT("Bold"))).Text(LOCTEXT("Back", "  EXIT TO TITLE  ")) ]
				]
			]
		]
	];
}

TSharedRef<SWidget> SSiteOverseerHub::BuildHeader()
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()
			[ SNew(STextBlock).Font(HubFont(26, TEXT("Bold"))).ColorAndOpacity(HubColors::Gray).Text(LOCTEXT("HubTitle", "SITE OVERSEER \u2014 FOUNDATION SITE-20")) ]
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(STextBlock).Font(HubFont(14)).ColorAndOpacity(HubColors::Cyan)
				.Text_Lambda([this]()
				{
					if (!Progress.IsValid()) return FText();
					const FRankData R = Progress->GetCurrentRank();
					return FText::FromString(FString::Printf(TEXT("CLEARANCE: %s  (Rank %d)   XP: %d"), *R.Title, R.Level, Progress->GetXP()));
				})
			]
		]
		+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(20, 0, 0, 0)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right)
			[
				SNew(STextBlock).Font(HubFont(18, TEXT("Bold"))).ColorAndOpacity(HubColors::Gold)
				.Text_Lambda([this]() { return Progress.IsValid() ? FText::FromString(FString::Printf(TEXT("CREDITS: %d"), Progress->GetCredits())) : FText(); })
			]
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right)
			[
				SNew(STextBlock).Font(HubFont(18, TEXT("Bold"))).ColorAndOpacity(HubColors::Cyan)
				.Text_Lambda([this]() { return Progress.IsValid() ? FText::FromString(FString::Printf(TEXT("RESEARCH CREDITS: %d"), Progress->GetResearchCredits())) : FText(); })
			]
		];
}

TSharedRef<SWidget> SSiteOverseerHub::BuildTabBar()
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)[ BuildTabButton(TEXT("RESEARCH DIVISION"), ETab::Research) ]
		+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)[ BuildTabButton(TEXT("STORE"), ETab::Store) ]
		+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)[ BuildTabButton(TEXT("CONTAINMENT OPERATIONS"), ETab::Operations) ]
		+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)[ BuildTabButton(TEXT("BREACH"), ETab::Breach) ]
		+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)[ BuildTabButton(TEXT("SITE-20 PERSONNEL"), ETab::Personnel) ]
		+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 8, 0)[ BuildTabButton(TEXT("ARCHIVES"), ETab::Archives) ]
		+ SHorizontalBox::Slot().AutoWidth()[ BuildTabButton(TEXT("RANKS & REWARDS"), ETab::Ranks) ];
}

TSharedRef<SWidget> SSiteOverseerHub::BuildTabButton(const FString& Label, ETab Tab)
{
	return SNew(SButton)
		.OnClicked_Lambda([this, Tab]() { SetTab(Tab); return FReply::Handled(); })
		[
			SNew(STextBlock)
			.Font(HubFont(13, TEXT("Bold")))
			.Text(FText::FromString(FString::Printf(TEXT("  %s  "), *Label)))
			.ColorAndOpacity_Lambda([this, Tab]() { return CurrentTab == Tab ? FSlateColor(HubColors::Cyan) : FSlateColor(HubColors::Dim); })
		];
}

void SSiteOverseerHub::SetTab(ETab Tab)
{
	CurrentTab = Tab;
	StatusMsg.Empty();
	RefreshContent();
}

void SSiteOverseerHub::RefreshContent()
{
	if (ContentBox.IsValid())
	{
		ContentBox->SetContent(BuildCurrentTabContent());
	}
}

TSharedRef<SWidget> SSiteOverseerHub::BuildCurrentTabContent()
{
	switch (CurrentTab)
	{
	case ETab::Research:   return BuildResearchTab();
	case ETab::Store:      return BuildStoreTab();
	case ETab::Operations: return BuildOperationsTab();
	case ETab::Breach:     return BuildBreachTab();
	case ETab::Personnel:  return BuildPersonnelTab();
	case ETab::Archives:   return BuildArchivesTab();
	case ETab::Ranks:      return BuildRanksTab();
	}
	return SNew(SSpacer);
}

TSharedRef<SWidget> SSiteOverseerHub::MakeRow(const FString& Title, const FString& Subtitle, const FString& RightInfo,
	const FString& ButtonLabel, bool bEnabled, const FLinearColor& AccentColor, FOnClicked OnClick)
{
	return SNew(SBorder)
		.BorderImage(&RowBg)
		.Padding(FMargin(14, 10))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
				[ SNew(STextBlock).Font(HubFont(15, TEXT("Bold"))).ColorAndOpacity(AccentColor).Text(FText::FromString(Title)) ]
				+ SVerticalBox::Slot().AutoHeight()
				[ SNew(STextBlock).Font(HubFont(11)).ColorAndOpacity(HubColors::Gray).AutoWrapText(true).Text(FText::FromString(Subtitle)) ]
			]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(16, 0)
			[ SNew(STextBlock).Font(HubFont(12, TEXT("Bold"))).ColorAndOpacity(HubColors::Gold).Text(FText::FromString(RightInfo)) ]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[
				SNew(SBox).MinDesiredWidth(150.f)
				[
					SNew(SButton)
					.IsEnabled(bEnabled)
					.HAlign(HAlign_Center)
					.OnClicked(OnClick)
					[ SNew(STextBlock).Font(HubFont(12, TEXT("Bold"))).Text(FText::FromString(ButtonLabel)) ]
				]
			]
		];
}

TSharedRef<SWidget> SSiteOverseerHub::BuildResearchTab()
{
	TSharedRef<SScrollBox> List = SNew(SScrollBox);

	List->AddSlot().Padding(0, 0, 0, 10)
	[
		SNew(STextBlock).Font(HubFont(13)).ColorAndOpacity(HubColors::Cyan).AutoWrapText(true)
		.Text(LOCTEXT("ResObj", "RESEARCH OBJECTIVES: Authorise field & research equipment and develop containment procedures. Spend RESEARCH CREDITS to complete projects \u2014 each unlocks an item for purchase in the Store or a containment procedure for Operations."))
	];

	for (const FResearchProject& P : UFoundationDataLibrary::GetResearchProjects())
	{
		const bool bDone = Progress.IsValid() && Progress->IsResearched(P.Id);
		const bool bRank = Progress.IsValid() && Progress->GetRankLevel() >= P.RequiredRank;
		const bool bAfford = Progress.IsValid() && Progress->GetResearchCredits() >= P.ResearchCost;
		const bool bEnabled = !bDone && bRank && bAfford;

		const FString Cat = (P.Category == EResearchCategory::Equipment) ? TEXT("[EQUIPMENT]") : TEXT("[CONTAINMENT PROCEDURE]");
		const FString Sub = FString::Printf(TEXT("%s  %s"), *Cat, *P.Objective);
		const FString Right = FString::Printf(TEXT("%d RC  \u2022  Rank %d"), P.ResearchCost, P.RequiredRank);
		FString Btn = bDone ? TEXT("RESEARCHED") : (!bRank ? TEXT("RANK LOCKED") : (!bAfford ? TEXT("INSUFFICIENT RC") : TEXT("RESEARCH")));
		const FLinearColor Accent = bDone ? HubColors::Green : HubColors::Cyan;

		const FName Id = P.Id;
		List->AddSlot().Padding(0, 0, 0, 6)
		[ MakeRow(P.Name, Sub, Right, Btn, bEnabled, Accent, FOnClicked::CreateSP(this, &SSiteOverseerHub::DoResearch, Id)) ];
	}
	return List;
}

TSharedRef<SWidget> SSiteOverseerHub::BuildStoreTab()
{
	TSharedRef<SScrollBox> List = SNew(SScrollBox);

	List->AddSlot().Padding(0, 0, 0, 10)
	[
		SNew(STextBlock).Font(HubFont(13)).ColorAndOpacity(HubColors::Gold).AutoWrapText(true)
		.Text(LOCTEXT("StoreObj", "REQUISITION STORE: Purchase equipment with CREDITS. Level-locked items remain LOCKED until their research project is complete in the Research Division."))
	];

	for (const FEquipmentData& E : UFoundationDataLibrary::GetEquipment())
	{
		const bool bOwned = Progress.IsValid() && Progress->IsOwned(E.Id);
		const bool bResearched = Progress.IsValid() && Progress->IsEquipmentResearched(E.Id);
		const bool bRank = Progress.IsValid() && Progress->GetRankLevel() >= E.RequiredRank;
		const bool bAfford = Progress.IsValid() && Progress->GetCredits() >= E.PurchaseCost;
		const bool bEnabled = !bOwned && bResearched && bRank && bAfford;

		const FString Dept = (E.Dept == EEquipDept::Field) ? TEXT("[FIELD]") : TEXT("[RESEARCH DEPT]");
		const FString Sub = FString::Printf(TEXT("%s  %s"), *Dept, *E.Description);
		const FString Right = FString::Printf(TEXT("%d cr  \u2022  Rank %d"), E.PurchaseCost, E.RequiredRank);

		FString Btn;
		FLinearColor Accent = HubColors::Gray;
		if (bOwned)             { Btn = TEXT("OWNED"); Accent = HubColors::Green; }
		else if (!bResearched)  { Btn = TEXT("RESEARCH REQ'D"); Accent = HubColors::Red; }
		else if (!bRank)        { Btn = TEXT("RANK LOCKED"); Accent = HubColors::Red; }
		else if (!bAfford)      { Btn = TEXT("NEED CREDITS"); Accent = HubColors::Gold; }
		else                    { Btn = TEXT("PURCHASE"); Accent = HubColors::Gold; }

		const FName Id = E.Id;
		List->AddSlot().Padding(0, 0, 0, 6)
		[ MakeRow(E.Name, Sub, Right, Btn, bEnabled, Accent, FOnClicked::CreateSP(this, &SSiteOverseerHub::DoPurchase, Id)) ];
	}
	return List;
}

TSharedRef<SWidget> SSiteOverseerHub::BuildOperationsTab()
{
	TSharedRef<SScrollBox> List = SNew(SScrollBox);

	List->AddSlot().Padding(0, 0, 0, 10)
	[
		SNew(STextBlock).Font(HubFont(13)).ColorAndOpacity(HubColors::Cyan).AutoWrapText(true)
		.Text(LOCTEXT("OpsObj", "CONTAINMENT OPERATIONS: Dispatch teams to contain anomalies and neutralise rogue MTF / GOI threats. Meet rank, equipment and procedure requirements, then dispatch to earn Credits, Research Credits and XP. (Separate from live Breach events.)"))
	];

	for (const FContainmentOp& O : UFoundationDataLibrary::GetOperations())
	{
		const bool bDone = Progress.IsValid() && Progress->IsOpComplete(O.Id);
		const bool bRank = Progress.IsValid() && Progress->GetRankLevel() >= O.RequiredRank;
		const bool bResearch = (O.RequiredResearch == NAME_None) || (Progress.IsValid() && Progress->IsResearched(O.RequiredResearch));
		const bool bEquip = (O.RequiredEquipment == NAME_None) || (Progress.IsValid() && Progress->IsOwned(O.RequiredEquipment));
		const bool bEnabled = !bDone && bRank && bResearch && bEquip;

		FString Req = FString::Printf(TEXT("Rank %d"), O.RequiredRank);
		if (O.RequiredEquipment != NAME_None)
		{
			const FEquipmentData* Eq = UFoundationDataLibrary::FindEquipment(O.RequiredEquipment);
			Req += FString::Printf(TEXT(" \u2022 Needs: %s"), Eq ? *Eq->Name : *O.RequiredEquipment.ToString());
		}
		const FString Sub = FString::Printf(TEXT("%s\nDifficulty %d/5  \u2022  %s"), *O.TargetDesignation, O.Difficulty, *Req);
		FString Right = FString::Printf(TEXT("+%d cr  +%d RC  +%d XP"), O.RewardCredits, O.RewardResearchCredits, O.RewardXP);
		if (bEnabled && Progress.IsValid())
		{
			Right += FString::Printf(TEXT("  |  %d%% success"), Progress->GetOperationSuccessChance(O.Id));
		}

		FString Btn;
		FLinearColor Accent = HubColors::Cyan;
		if (bDone)            { Btn = TEXT("CONTAINED"); Accent = HubColors::Green; }
		else if (!bRank)      { Btn = TEXT("RANK LOCKED"); Accent = HubColors::Red; }
		else if (!bEquip || !bResearch) { Btn = TEXT("REQS NOT MET"); Accent = HubColors::Red; }
		else                  { Btn = TEXT("DISPATCH"); Accent = HubColors::Cyan; }

		const FName Id = O.Id;
		List->AddSlot().Padding(0, 0, 0, 6)
		[ MakeRow(O.Name, Sub, Right, Btn, bEnabled, Accent, FOnClicked::CreateSP(this, &SSiteOverseerHub::DoOperation, Id)) ];
	}
	return List;
}

TSharedRef<SWidget> SSiteOverseerHub::BuildBreachTab()
{
	TSharedRef<SScrollBox> List = SNew(SScrollBox);

	List->AddSlot().Padding(0, 0, 0, 8)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
		[
			SNew(STextBlock).Font(HubFont(13)).ColorAndOpacity(HubColors::Red).AutoWrapText(true)
			.Text(LOCTEXT("BreachObj", "CONTAINMENT BREACH (LIVE): Deploy into the facility first-person. All anomalies below can breach \u2014 survive, re-contain them and escape. Rewards feed your rank."))
		]
		+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(16, 0, 0, 0)
		[
			SNew(SButton)
			.OnClicked(this, &SSiteOverseerHub::DoDeployBreach)
			[ SNew(STextBlock).Font(HubFont(16, TEXT("Bold"))).ColorAndOpacity(HubColors::Red).Text(LOCTEXT("Deploy", "  \u26A0  DEPLOY TO BREACH  ")) ]
		]
	];

	List->AddSlot().Padding(0, 0, 0, 10)
	[
		SNew(STextBlock).Font(HubFont(12)).ColorAndOpacity(HubColors::Gold)
		.Text_Lambda([this]()
		{
			if (!Progress.IsValid()) return FText();
			return FText::FromString(FString::Printf(TEXT("BREACH RECORD \u2014 Survived: %d   Failed: %d   SCPs Re-contained: %d"),
				Progress->GetBreachesSurvived(), Progress->GetBreachesFailed(), Progress->GetSCPsRecontained()));
		})
	];

	for (const FSCPData& S : UFoundationDataLibrary::GetSCPs())
	{
		if (!S.bBreachable) continue;
		const FString Sub = FString::Printf(TEXT("[%s]  Threat %d/5  \u2022  %s"),
			*UFoundationDataLibrary::SCPClassToString(S.Class), S.ThreatLevel, *S.ContainmentProcedure);
		FLinearColor Accent = HubColors::Gray;
		switch (S.Class)
		{
		case ESCPClass::Keter: Accent = HubColors::Red; break;
		case ESCPClass::Euclid: Accent = HubColors::Gold; break;
		case ESCPClass::Safe: Accent = HubColors::Green; break;
		default: Accent = HubColors::Cyan; break;
		}
		List->AddSlot().Padding(0, 0, 0, 6)
		[
			SNew(SBorder).BorderImage(&RowBg).Padding(FMargin(14, 8))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
				[ SNew(STextBlock).Font(HubFont(15, TEXT("Bold"))).ColorAndOpacity(Accent).Text(FText::FromString(FString::Printf(TEXT("%s \u2014 %s"), *S.Number, *S.Name))) ]
				+ SVerticalBox::Slot().AutoHeight()
				[ SNew(STextBlock).Font(HubFont(11)).ColorAndOpacity(HubColors::Gray).AutoWrapText(true).Text(FText::FromString(Sub)) ]
			]
		];
	}
	return List;
}

TSharedRef<SWidget> SSiteOverseerHub::BuildPersonnelTab()
{
	TSharedRef<SScrollBox> List = SNew(SScrollBox);

	List->AddSlot().Padding(0, 0, 0, 10)
	[
		SNew(STextBlock).Font(HubFont(13)).ColorAndOpacity(HubColors::Cyan).AutoWrapText(true)
		.Text(LOCTEXT("PersObj", "SITE-20 PERSONNEL DOSSIERS \u2014 Level 5 / O5 Eyes Only. Key staff overseeing Foundation Site-20."))
	];

	auto Line = [](const FString& Label, const FString& Value, const FLinearColor& ValueColor) -> TSharedRef<SWidget>
	{
		return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Top)
			[ SNew(SBox).MinDesiredWidth(150.f)[ SNew(STextBlock).Font(HubFont(11, TEXT("Bold"))).ColorAndOpacity(HubColors::Dim).Text(FText::FromString(Label)) ] ]
			+ SHorizontalBox::Slot().FillWidth(1.f)
			[ SNew(STextBlock).Font(HubFont(12)).ColorAndOpacity(ValueColor).AutoWrapText(true).Text(FText::FromString(Value)) ];
	};

	for (const FPersonnelData& P : UFoundationDataLibrary::GetPersonnel())
	{
		List->AddSlot().Padding(0, 0, 0, 10)
		[
			SNew(SBorder).BorderImage(&RowBg).Padding(FMargin(16, 12))
			[
				SNew(SVerticalBox)
				// Header: codename + file no
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
					[ SNew(STextBlock).Font(HubFont(22, TEXT("Bold"))).ColorAndOpacity(HubColors::Cyan).Text(FText::FromString(P.Codename)) ]
					+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
					[ SNew(STextBlock).Font(HubFont(11)).ColorAndOpacity(HubColors::Dim).Text(FText::FromString(P.FileNo)) ]
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 2, 0, 8)
				[ SNew(STextBlock).Font(HubFont(13, TEXT("Bold"))).ColorAndOpacity(HubColors::Gold).Text(FText::FromString(P.Position)) ]

				+ SVerticalBox::Slot().AutoHeight().Padding(0, 1)[ Line(TEXT("REAL NAME"), P.RealName, HubColors::Gray) ]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 1)[ Line(TEXT("CLEARANCE"), P.Clearance, HubColors::Gold) ]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 1)[ Line(TEXT("STATUS"), P.Status, HubColors::Green) ]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 1)[ Line(TEXT("NATIONALITY"), P.Nationality, HubColors::Gray) ]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 1)[ Line(TEXT("AFFILIATION"), P.Affiliation, HubColors::Gray) ]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 1)[ Line(TEXT("ALIASES"), P.Aliases, HubColors::Gray) ]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 1)[ Line(TEXT("ANOMALOUS TRAIT"), P.AnomalousTrait, HubColors::Red) ]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 8, 0, 1)[ Line(TEXT("SERVICE SUMMARY"), P.Bio, HubColors::Gray) ]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 6, 0, 1)[ Line(TEXT("O5 NOTES"), P.O5Notes, HubColors::Cyan) ]
			]
		];
	}
	return List;
}

TSharedRef<SWidget> SSiteOverseerHub::BuildArchivesTab()
{
	TSharedRef<SScrollBox> List = SNew(SScrollBox);

	List->AddSlot().Padding(0, 0, 0, 10)
	[
		SNew(STextBlock).Font(HubFont(13)).ColorAndOpacity(HubColors::Cyan).AutoWrapText(true)
		.Text(LOCTEXT("ArchObj", "SITE-20 ARCHIVES \u2014 Author and file your own SCP reports & dossiers. Entries are saved to a local JSON archive and persist between sessions."))
	];

	// --- New entry form ---
	List->AddSlot().Padding(0, 0, 0, 14)
	[
		SNew(SBorder).BorderImage(&RowBg).Padding(FMargin(16, 12))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 6)
			[ SNew(STextBlock).Font(HubFont(15, TEXT("Bold"))).ColorAndOpacity(HubColors::Gold).Text(LOCTEXT("NewReport", "FILE NEW REPORT")) ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
			[ SAssignNew(DesignationBox, SEditableTextBox).HintText(LOCTEXT("hDes", "Designation (e.g. SCP-XXXX)")) ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
			[ SAssignNew(NameBox, SEditableTextBox).HintText(LOCTEXT("hName", "Name / Title")) ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
			[ SAssignNew(ClassBox, SEditableTextBox).HintText(LOCTEXT("hClass", "Object Class (Safe / Euclid / Keter...)")) ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
			[
				SNew(SBox).HeightOverride(110.f)
				[ SAssignNew(BodyBox, SMultiLineEditableTextBox).HintText(LOCTEXT("hBody", "Description / Special Containment Procedures")) ]
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 8, 0, 0).HAlign(HAlign_Left)
			[
				SNew(SBox).MinDesiredWidth(220.f)
				[
					SNew(SButton).HAlign(HAlign_Center).OnClicked(this, &SSiteOverseerHub::DoSaveArchive)
					[ SNew(STextBlock).Font(HubFont(13, TEXT("Bold"))).Text(LOCTEXT("SaveArch", "SAVE TO ARCHIVE")) ]
				]
			]
		]
	];

	// --- Existing entries ---
	if (Archive.IsValid())
	{
		const TArray<FArchiveEntry>& Entries = Archive->GetEntries();
		if (Entries.Num() == 0)
		{
			List->AddSlot().Padding(0, 4)
			[ SNew(STextBlock).Font(HubFont(12)).ColorAndOpacity(HubColors::Dim).Text(LOCTEXT("NoArch", "No archived reports yet. File your first above.")) ];
		}
		for (int32 i = 0; i < Entries.Num(); ++i)
		{
			const FArchiveEntry& E = Entries[i];
			const int32 Idx = i;
			List->AddSlot().Padding(0, 0, 0, 6)
			[
				SNew(SBorder).BorderImage(&RowBg).Padding(FMargin(14, 10))
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot().AutoHeight()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
						[ SNew(STextBlock).Font(HubFont(16, TEXT("Bold"))).ColorAndOpacity(HubColors::Cyan).Text(FText::FromString(FString::Printf(TEXT("%s \u2014 %s"), *E.Designation, *E.Name))) ]
						+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
						[
							SNew(SBox).MinDesiredWidth(110.f)
							[
								SNew(SButton).HAlign(HAlign_Center).OnClicked(FOnClicked::CreateSP(this, &SSiteOverseerHub::DoDeleteArchive, Idx))
								[ SNew(STextBlock).Font(HubFont(11, TEXT("Bold"))).ColorAndOpacity(HubColors::Red).Text(LOCTEXT("Del", "DELETE")) ]
							]
						]
					]
					+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
					[ SNew(STextBlock).Font(HubFont(11, TEXT("Bold"))).ColorAndOpacity(HubColors::Gold).Text(FText::FromString(FString::Printf(TEXT("Object Class: %s   \u2022   Filed: %s   \u2022   By: %s"), *E.ObjectClass, *E.Timestamp, *E.Author))) ]
					+ SVerticalBox::Slot().AutoHeight().Padding(0, 4, 0, 0)
					[ SNew(STextBlock).Font(HubFont(12)).ColorAndOpacity(HubColors::Gray).AutoWrapText(true).Text(FText::FromString(E.Body)) ]
				]
			];
		}
	}
	return List;
}

FReply SSiteOverseerHub::DoSaveArchive()
{
	if (!Archive.IsValid())
	{
		StatusMsg = TEXT("Archive unavailable.");
		return FReply::Handled();
	}

	const FString Des = DesignationBox.IsValid() ? DesignationBox->GetText().ToString().TrimStartAndEnd() : FString();
	if (Des.IsEmpty())
	{
		StatusMsg = TEXT("Designation is required to file a report.");
		RefreshContent();
		return FReply::Handled();
	}

	FArchiveEntry E;
	E.Designation = Des;
	E.Name = NameBox.IsValid() ? NameBox->GetText().ToString() : FString();
	E.ObjectClass = ClassBox.IsValid() ? ClassBox->GetText().ToString() : FString();
	E.Body = BodyBox.IsValid() ? BodyBox->GetText().ToString() : FString();
	E.Author = Progress.IsValid() ? Progress->GetCurrentRank().Title : FString(TEXT("Overseer"));
	E.Timestamp = FDateTime::Now().ToString();

	Archive->AddEntry(E);
	StatusMsg = FString::Printf(TEXT("Archived %s and saved to disk."), *Des);
	RefreshContent();
	return FReply::Handled();
}

FReply SSiteOverseerHub::DoDeleteArchive(int32 Index)
{
	if (Archive.IsValid() && Archive->RemoveEntry(Index))
	{
		StatusMsg = TEXT("Archive entry deleted.");
	}
	RefreshContent();
	return FReply::Handled();
}

TSharedRef<SWidget> SSiteOverseerHub::BuildRanksTab()
{
	TSharedRef<SScrollBox> List = SNew(SScrollBox);

	List->AddSlot().Padding(0, 0, 0, 10)
	[
		SNew(STextBlock).Font(HubFont(13)).ColorAndOpacity(HubColors::Cyan).AutoWrapText(true)
		.Text_Lambda([this]()
		{
			if (!Progress.IsValid()) return FText();
			const FRankData Cur = Progress->GetCurrentRank();
			FString Line = FString::Printf(TEXT("CURRENT CLEARANCE: %s (Rank %d)   XP: %d"), *Cur.Title, Cur.Level, Progress->GetXP());
			if (Progress->HasNextRank())
			{
				const FRankData Next = Progress->GetNextRank();
				Line += FString::Printf(TEXT("\nNEXT: %s at %d XP  (+%d cr, +%d RC reward)"), *Next.Title, Next.XPRequired, Next.CreditReward, Next.ResearchCreditReward);
			}
			else { Line += TEXT("\nMAXIMUM CLEARANCE REACHED \u2014 OMEGA BLACK."); }
			return FText::FromString(Line);
		})
	];

	// Breach section inside Ranks & Rewards.
	List->AddSlot().Padding(0, 0, 0, 12)
	[
		SNew(SBorder).BorderImage(&RowBg).Padding(FMargin(14, 10))
		[
			SNew(STextBlock).Font(HubFont(13, TEXT("Bold"))).ColorAndOpacity(HubColors::Red)
			.Text_Lambda([this]()
			{
				if (!Progress.IsValid()) return FText();
				return FText::FromString(FString::Printf(TEXT("BREACH SECTION \u2014 Survived: %d  |  Failed: %d  |  Re-contained: %d  (each survival rewards cr/RC/XP)"),
					Progress->GetBreachesSurvived(), Progress->GetBreachesFailed(), Progress->GetSCPsRecontained()));
			})
		]
	];

	for (const FRankData& R : UFoundationDataLibrary::GetRanks())
	{
		const bool bAchieved = Progress.IsValid() && Progress->GetRankLevel() >= R.Level;
		const FLinearColor Accent = bAchieved ? HubColors::Green : HubColors::Dim;
		const FString Title = FString::Printf(TEXT("Rank %d \u2014 %s"), R.Level, *R.Title);
		const FString Sub = FString::Printf(TEXT("Requires %d XP   \u2022   Promotion reward: +%d cr, +%d RC"), R.XPRequired, R.CreditReward, R.ResearchCreditReward);
		List->AddSlot().Padding(0, 0, 0, 6)
		[
			SNew(SBorder).BorderImage(&RowBg).Padding(FMargin(14, 8))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
				[ SNew(STextBlock).Font(HubFont(15, TEXT("Bold"))).ColorAndOpacity(Accent).Text(FText::FromString(bAchieved ? Title + TEXT("  \u2713") : Title)) ]
				+ SVerticalBox::Slot().AutoHeight()
				[ SNew(STextBlock).Font(HubFont(11)).ColorAndOpacity(HubColors::Gray).Text(FText::FromString(Sub)) ]
			]
		];
	}
	return List;
}

FReply SSiteOverseerHub::DoResearch(FName ProjectId)
{
	if (Progress.IsValid()) { Progress->TryCompleteResearch(ProjectId, StatusMsg); }
	RefreshContent();
	return FReply::Handled();
}

FReply SSiteOverseerHub::DoPurchase(FName EquipId)
{
	if (Progress.IsValid()) { Progress->TryPurchase(EquipId, StatusMsg); }
	RefreshContent();
	return FReply::Handled();
}

FReply SSiteOverseerHub::DoOperation(FName OpId)
{
	if (Progress.IsValid()) { Progress->TryRunOperation(OpId, StatusMsg); }
	RefreshContent();
	return FReply::Handled();
}

FReply SSiteOverseerHub::DoDeployBreach()
{
	OnDeployBreach.ExecuteIfBound();
	return FReply::Handled();
}

FReply SSiteOverseerHub::DoBackToTitle()
{
	OnBackToTitle.ExecuteIfBound();
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
