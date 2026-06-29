#include "OverseerProgression.h"
#include "OverseerSaveGame.h"
#include "FoundationDataLibrary.h"
#include "Kismet/GameplayStatics.h"

void UOverseerProgression::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UGameplayStatics::DoesSaveGameExist(SlotName(), 0))
	{
		Data = Cast<UOverseerSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName(), 0));
	}

	if (!Data)
	{
		Data = Cast<UOverseerSaveGame>(UGameplayStatics::CreateSaveGameObject(UOverseerSaveGame::StaticClass()));
		Save();
	}
}

void UOverseerProgression::Save()
{
	if (Data)
	{
		UGameplayStatics::SaveGameToSlot(Data, SlotName(), 0);
	}
}

int32 UOverseerProgression::GetCredits() const { return Data ? Data->Credits : 0; }
int32 UOverseerProgression::GetResearchCredits() const { return Data ? Data->ResearchCredits : 0; }
int32 UOverseerProgression::GetXP() const { return Data ? Data->XP : 0; }
int32 UOverseerProgression::GetRankLevel() const { return Data ? Data->RankLevel : 0; }

FRankData UOverseerProgression::GetCurrentRank() const
{
	return UFoundationDataLibrary::GetRankData(GetRankLevel());
}

bool UOverseerProgression::HasNextRank() const
{
	return GetRankLevel() + 1 < UFoundationDataLibrary::GetRanks().Num();
}

FRankData UOverseerProgression::GetNextRank() const
{
	return UFoundationDataLibrary::GetRankData(GetRankLevel() + 1);
}

bool UOverseerProgression::IsResearched(FName ProjectId) const
{
	return Data && Data->CompletedResearch.Contains(ProjectId);
}

bool UOverseerProgression::IsEquipmentResearched(FName EquipId) const
{
	const FEquipmentData* E = UFoundationDataLibrary::FindEquipment(EquipId);
	if (!E) return false;
	if (!E->bRequiresResearch) return true;
	return IsResearched(UFoundationDataLibrary::ResearchIdForEquipment(EquipId));
}

bool UOverseerProgression::IsOwned(FName EquipId) const
{
	return Data && Data->OwnedEquipment.Contains(EquipId);
}

bool UOverseerProgression::IsOpComplete(FName OpId) const
{
	return Data && Data->CompletedOps.Contains(OpId);
}

int32 UOverseerProgression::GetOperationSuccessChance(FName OpId) const
{
	const FContainmentOp* Op = nullptr;
	for (const FContainmentOp& O : UFoundationDataLibrary::GetOperations())
	{
		if (O.Id == OpId) { Op = &O; break; }
	}
	if (!Op || !Data) { return 0; }

	const int32 RankAbove = FMath::Max(0, Data->RankLevel - Op->RequiredRank);
	const int32 Chance = 90 - Op->Difficulty * 8 + RankAbove * 10;
	return FMath::Clamp(Chance, 25, 95);
}

int32 UOverseerProgression::GetBreachesSurvived() const { return Data ? Data->BreachesSurvived : 0; }
int32 UOverseerProgression::GetBreachesFailed() const { return Data ? Data->BreachesFailed : 0; }
int32 UOverseerProgression::GetSCPsRecontained() const { return Data ? Data->SCPsRecontained : 0; }

const TArray<FName>& UOverseerProgression::GetLoadout() const
{
	static const TArray<FName> Empty;
	return Data ? Data->Loadout : Empty;
}

bool UOverseerProgression::IsInLoadout(FName EquipId) const
{
	return Data && Data->Loadout.Contains(EquipId);
}

bool UOverseerProgression::ToggleLoadout(FName EquipId, FString& OutMsg)
{
	if (!Data) { OutMsg = TEXT("Save unavailable."); return false; }
	if (!IsOwned(EquipId)) { OutMsg = TEXT("You must own this equipment first."); return false; }

	if (Data->Loadout.Contains(EquipId))
	{
		Data->Loadout.Remove(EquipId);
		Save();
		OutMsg = TEXT("Removed from loadout.");
		return true;
	}
	if (Data->Loadout.Num() >= GetLoadoutCapacity())
	{
		OutMsg = FString::Printf(TEXT("Loadout full (%d slots)."), GetLoadoutCapacity());
		return false;
	}
	Data->Loadout.Add(EquipId);
	Save();
	OutMsg = TEXT("Added to loadout.");
	return true;
}

bool UOverseerProgression::TryCompleteResearch(FName ProjectId, FString& OutMsg)
{
	if (!Data) { OutMsg = TEXT("Save unavailable."); return false; }

	const FResearchProject* P = UFoundationDataLibrary::FindResearch(ProjectId);
	if (!P) { OutMsg = TEXT("Unknown research project."); return false; }
	if (IsResearched(ProjectId)) { OutMsg = TEXT("Already researched."); return false; }
	if (Data->RankLevel < P->RequiredRank) { OutMsg = FString::Printf(TEXT("Requires rank %d."), P->RequiredRank); return false; }
	if (Data->ResearchCredits < P->ResearchCost) { OutMsg = TEXT("Not enough Research Credits."); return false; }

	Data->ResearchCredits -= P->ResearchCost;
	Data->CompletedResearch.Add(ProjectId);
	Save();
	OutMsg = FString::Printf(TEXT("Research complete: %s"), *P->Name);
	return true;
}

bool UOverseerProgression::TryPurchase(FName EquipId, FString& OutMsg)
{
	if (!Data) { OutMsg = TEXT("Save unavailable."); return false; }

	const FEquipmentData* E = UFoundationDataLibrary::FindEquipment(EquipId);
	if (!E) { OutMsg = TEXT("Unknown item."); return false; }
	if (IsOwned(EquipId)) { OutMsg = TEXT("Already owned."); return false; }
	if (E->bRequiresResearch && !IsEquipmentResearched(EquipId)) { OutMsg = TEXT("Research required before purchase."); return false; }
	if (Data->RankLevel < E->RequiredRank) { OutMsg = FString::Printf(TEXT("Requires rank %d."), E->RequiredRank); return false; }
	if (Data->Credits < E->PurchaseCost) { OutMsg = TEXT("Not enough Credits."); return false; }

	Data->Credits -= E->PurchaseCost;
	Data->OwnedEquipment.Add(EquipId);
	Save();
	OutMsg = FString::Printf(TEXT("Purchased: %s"), *E->Name);
	return true;
}

bool UOverseerProgression::TryRunOperation(FName OpId, FString& OutMsg)
{
	if (!Data) { OutMsg = TEXT("Save unavailable."); return false; }

	const FContainmentOp* Op = nullptr;
	for (const FContainmentOp& O : UFoundationDataLibrary::GetOperations())
	{
		if (O.Id == OpId) { Op = &O; break; }
	}
	if (!Op) { OutMsg = TEXT("Unknown operation."); return false; }
	if (IsOpComplete(OpId)) { OutMsg = TEXT("Operation already completed."); return false; }
	if (Data->RankLevel < Op->RequiredRank) { OutMsg = FString::Printf(TEXT("Requires rank %d."), Op->RequiredRank); return false; }
	if (Op->RequiredResearch != NAME_None && !IsResearched(Op->RequiredResearch)) { OutMsg = TEXT("Required containment procedure not researched."); return false; }
	if (Op->RequiredEquipment != NAME_None && !IsOwned(Op->RequiredEquipment))
	{
		const FEquipmentData* Req = UFoundationDataLibrary::FindEquipment(Op->RequiredEquipment);
		OutMsg = FString::Printf(TEXT("Requires equipment: %s"), Req ? *Req->Name : *Op->RequiredEquipment.ToString());
		return false;
	}

	const int32 Chance = GetOperationSuccessChance(OpId);
	const int32 Roll = FMath::RandRange(1, 100);
	if (Roll > Chance)
	{
		const int32 PartialXP = Op->RewardXP / 4;
		AddRewards(0, 0, PartialXP);
		Save();
		OutMsg = FString::Printf(TEXT("Operation FAILED (%d%% chance). Team withdrew. +%d XP field experience."), Chance, PartialXP);
		return false;
	}

	Data->CompletedOps.Add(OpId);
	AddRewards(Op->RewardCredits, Op->RewardResearchCredits, Op->RewardXP);
	Save();
	OutMsg = FString::Printf(TEXT("Operation SUCCESS (%d%% chance): +%d cr, +%d RC, +%d XP"), Chance, Op->RewardCredits, Op->RewardResearchCredits, Op->RewardXP);
	return true;
}

void UOverseerProgression::RecordBreachResult(bool bSurvived, int32 Recontained)
{
	if (!Data) return;

	if (bSurvived)
	{
		Data->BreachesSurvived++;
		Data->SCPsRecontained += FMath::Max(0, Recontained);
		AddRewards(300 + 100 * Recontained, 120 + 40 * Recontained, 400 + 150 * Recontained);
	}
	else
	{
		Data->BreachesFailed++;
		AddRewards(50, 20, 75); // hazard pay even on failure
	}
	Save();
}

void UOverseerProgression::AddRewards(int32 Credits, int32 ResearchCredits, int32 XP)
{
	if (!Data) return;
	Data->Credits += Credits;
	Data->ResearchCredits += ResearchCredits;
	Data->XP += XP;
	CheckRankUp();
}

void UOverseerProgression::CheckRankUp()
{
	if (!Data) return;
	const TArray<FRankData>& Ranks = UFoundationDataLibrary::GetRanks();
	while (Data->RankLevel + 1 < Ranks.Num() && Data->XP >= Ranks[Data->RankLevel + 1].XPRequired)
	{
		Data->RankLevel++;
		const FRankData& R = Ranks[Data->RankLevel];
		Data->Credits += R.CreditReward;
		Data->ResearchCredits += R.ResearchCreditReward;
	}
}
