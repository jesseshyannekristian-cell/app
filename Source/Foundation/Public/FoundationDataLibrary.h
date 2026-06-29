#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FoundationTypes.h"
#include "FoundationDataLibrary.generated.h"

/**
 * Static, code-defined database for the Site Overseer meta-game.
 * Curated, lore-accurate seed data. Operations & equipment research projects are
 * generated from the SCP / equipment lists so every anomaly and every level-locked
 * item is automatically covered.
 */
UCLASS()
class FOUNDATION_API UFoundationDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static const TArray<FSCPData>& GetSCPs();
	static const TArray<FRogueMTFData>& GetRogueMTF();
	static const TArray<FEquipmentData>& GetEquipment();
	static const TArray<FResearchProject>& GetResearchProjects();
	static const TArray<FRankData>& GetRanks();
	static const TArray<FContainmentOp>& GetOperations();

	static const FEquipmentData* FindEquipment(FName Id);
	static const FResearchProject* FindResearch(FName Id);
	static const FRankData& GetRankData(int32 Level);

	/** Research project id that unlocks a given equipment ("res_<equipId>"). */
	static FName ResearchIdForEquipment(FName EquipId);

	static FString SCPClassToString(ESCPClass Class);
};
