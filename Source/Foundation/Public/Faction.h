#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Faction.generated.h"

UENUM(BlueprintType)
enum class EFaction : uint8
{
	Foundation		UMETA(DisplayName = "SCP Foundation"),
	MTF				UMETA(DisplayName = "Mobile Task Force"),
	ChaosInsurgency	UMETA(DisplayName = "Chaos Insurgency"),
	UIU				UMETA(DisplayName = "Unusual Incidents Unit"),
	Wondertainment	UMETA(DisplayName = "Dr. Wondertainment"),
	Anomaly			UMETA(DisplayName = "Anomalous Entity"),
	Civilian		UMETA(DisplayName = "Civilian")
};

UENUM(BlueprintType)
enum class EAttitude : uint8
{
	Hostile,
	Neutral,
	Friendly
};

UCLASS()
class FOUNDATION_API UFactionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Faction")
	static EAttitude GetAttitude(EFaction A, EFaction B);

	UFUNCTION(BlueprintCallable, Category = "Faction")
	static FLinearColor GetFactionColor(EFaction Faction);

	UFUNCTION(BlueprintCallable, Category = "Faction")
	static FString GetFactionName(EFaction Faction);
};
