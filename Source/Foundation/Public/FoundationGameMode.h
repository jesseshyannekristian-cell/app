#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FoundationGameMode.generated.h"

class AFacilityGenerator;

UCLASS()
class FOUNDATION_API AFoundationGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFoundationGameMode();

	virtual void BeginPlay() override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UPROPERTY(EditDefaultsOnly, Category = "Facility")
	TSubclassOf<AFacilityGenerator> GeneratorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Facility")
	int32 RestartDelay = 3;

	UFUNCTION(BlueprintCallable, Category = "Facility")
	void HandlePlayerDeath();

	UFUNCTION(BlueprintCallable, Category = "Facility")
	void HandlePlayerEscaped();

	/** Called by containment terminals when an SCP is successfully re-contained during a breach. */
	void NotifyRecontained();

protected:
	int32 SCPsRecontainedThisRun = 0;

	UPROPERTY()
	AFacilityGenerator* Generator;

	void RestartLevel();
};
