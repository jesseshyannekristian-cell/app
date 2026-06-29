#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "OverseerSaveGame.generated.h"

UCLASS()
class FOUNDATION_API UOverseerSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY() int32 Credits = 500;
	UPROPERTY() int32 ResearchCredits = 200;
	UPROPERTY() int32 XP = 0;
	UPROPERTY() int32 RankLevel = 0;

	UPROPERTY() TArray<FName> CompletedResearch;
	UPROPERTY() TArray<FName> OwnedEquipment;
	UPROPERTY() TArray<FName> CompletedOps;

	/** Equipment selected to carry into a breach (subset of OwnedEquipment). */
	UPROPERTY() TArray<FName> Loadout;

	UPROPERTY() int32 BreachesSurvived = 0;
	UPROPERTY() int32 BreachesFailed = 0;
	UPROPERTY() int32 SCPsRecontained = 0;
};
