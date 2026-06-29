#pragma once

#include "CoreMinimal.h"
#include "FoundationTypes.generated.h"

UENUM(BlueprintType)
enum class ESCPClass : uint8
{
	Safe,
	Euclid,
	Keter,
	Thaumiel,
	Apollyon,
	Neutralized
};

UENUM(BlueprintType)
enum class EEquipDept : uint8
{
	Field,
	Research
};

UENUM(BlueprintType)
enum class EResearchCategory : uint8
{
	Equipment,
	ContainmentProcedure
};

/** A single anomaly entry — used in Breach (containable) and Containment Operations. */
USTRUCT(BlueprintType)
struct FSCPData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FString Number;
	UPROPERTY(BlueprintReadOnly) FString Name;
	UPROPERTY(BlueprintReadOnly) ESCPClass Class = ESCPClass::Euclid;
	UPROPERTY(BlueprintReadOnly) FString ContainmentProcedure;
	UPROPERTY(BlueprintReadOnly) int32 ThreatLevel = 1; // 1-5
	UPROPERTY(BlueprintReadOnly) bool bBreachable = true;
};

/** Rogue / defected MTF & Group-of-Interest threats — neutralisation targets. */
USTRUCT(BlueprintType)
struct FRogueMTFData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FString Name;
	UPROPERTY(BlueprintReadOnly) FString Origin;
	UPROPERTY(BlueprintReadOnly) int32 ThreatLevel = 1;
	UPROPERTY(BlueprintReadOnly) FString Notes;
};

/** Purchasable equipment. Level-locked items must be researched before they appear in the store. */
USTRUCT(BlueprintType)
struct FEquipmentData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FName Id;
	UPROPERTY(BlueprintReadOnly) FString Name;
	UPROPERTY(BlueprintReadOnly) EEquipDept Dept = EEquipDept::Field;
	UPROPERTY(BlueprintReadOnly) FString Description;
	UPROPERTY(BlueprintReadOnly) int32 RequiredRank = 0;
	UPROPERTY(BlueprintReadOnly) int32 PurchaseCost = 0;        // standard credits
	UPROPERTY(BlueprintReadOnly) bool bRequiresResearch = true; // level-locked
};

/** A research project. Spends Research Credits. Either unlocks an equipment or a containment procedure. */
USTRUCT(BlueprintType)
struct FResearchProject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FName Id;
	UPROPERTY(BlueprintReadOnly) FString Name;
	UPROPERTY(BlueprintReadOnly) EResearchCategory Category = EResearchCategory::Equipment;
	UPROPERTY(BlueprintReadOnly) FString Objective;
	UPROPERTY(BlueprintReadOnly) FString Description;
	UPROPERTY(BlueprintReadOnly) int32 ResearchCost = 0; // research credits
	UPROPERTY(BlueprintReadOnly) int32 RequiredRank = 0;
	UPROPERTY(BlueprintReadOnly) FName UnlocksEquipment; // NAME_None for procedure projects
};

USTRUCT(BlueprintType)
struct FRankData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) int32 Level = 0;
	UPROPERTY(BlueprintReadOnly) FString Title;
	UPROPERTY(BlueprintReadOnly) int32 XPRequired = 0;
	UPROPERTY(BlueprintReadOnly) int32 CreditReward = 0;
	UPROPERTY(BlueprintReadOnly) int32 ResearchCreditReward = 0;
};

/** A menu-driven containment / neutralisation operation (NOT the FPS breach level). */
USTRUCT(BlueprintType)
struct FContainmentOp
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FName Id;
	UPROPERTY(BlueprintReadOnly) FString Name;
	UPROPERTY(BlueprintReadOnly) FString TargetDesignation;
	UPROPERTY(BlueprintReadOnly) FString Description;
	UPROPERTY(BlueprintReadOnly) int32 RequiredRank = 0;
	UPROPERTY(BlueprintReadOnly) FName RequiredEquipment; // NAME_None if none
	UPROPERTY(BlueprintReadOnly) FName RequiredResearch;  // research project id, NAME_None if none
	UPROPERTY(BlueprintReadOnly) int32 Difficulty = 1;    // 1-5
	UPROPERTY(BlueprintReadOnly) int32 RewardCredits = 0;
	UPROPERTY(BlueprintReadOnly) int32 RewardResearchCredits = 0;
	UPROPERTY(BlueprintReadOnly) int32 RewardXP = 0;
};
