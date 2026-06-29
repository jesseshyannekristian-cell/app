#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FoundationTypes.h"
#include "OverseerProgression.generated.h"

class UOverseerSaveGame;

/**
 * Persistent meta-game state: two currencies (Credits + Research Credits), rank/XP,
 * unlocked research, owned equipment, completed operations and breach record.
 * Auto-created with the GameInstance; loads/creates a save on Initialize.
 */
UCLASS()
class FOUNDATION_API UOverseerProgression : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void Save();

	// --- Queries ---
	int32 GetCredits() const;
	int32 GetResearchCredits() const;
	int32 GetXP() const;
	int32 GetRankLevel() const;
	FRankData GetCurrentRank() const;
	bool HasNextRank() const;
	FRankData GetNextRank() const;

	bool IsResearched(FName ProjectId) const;
	bool IsEquipmentResearched(FName EquipId) const;
	bool IsOwned(FName EquipId) const;
	bool IsOpComplete(FName OpId) const;

	int32 GetBreachesSurvived() const;
	int32 GetBreachesFailed() const;
	int32 GetSCPsRecontained() const;

	// --- Actions (return success + human-readable message) ---
	bool TryCompleteResearch(FName ProjectId, FString& OutMsg);
	bool TryPurchase(FName EquipId, FString& OutMsg);
	bool TryRunOperation(FName OpId, FString& OutMsg);
	void RecordBreachResult(bool bSurvived, int32 Recontained);

private:
	UPROPERTY()
	UOverseerSaveGame* Data = nullptr;

	void AddRewards(int32 Credits, int32 ResearchCredits, int32 XP);
	void CheckRankUp();

	static const TCHAR* SlotName() { return TEXT("OverseerSave"); }
};
