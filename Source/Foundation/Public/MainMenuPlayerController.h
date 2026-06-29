#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

class SMainMenuWidget;
class SSiteOverseerHub;

UCLASS()
class FOUNDATION_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	UTexture2D* TitleTexture = nullptr;

	TSharedPtr<SMainMenuWidget> TitleWidget;
	TSharedPtr<SSiteOverseerHub> HubWidget;

	void ShowTitle();
	void ShowHub();
	void RemoveAllWidgets();
	void ApplyUIInputMode();

	void OnStartGame();
	void OnQuit();
	void OnDeployBreach();
	void OnBackToTitle();
};
