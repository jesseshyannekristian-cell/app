#include "MainMenuPlayerController.h"
#include "SMainMenuWidget.h"
#include "SSiteOverseerHub.h"
#include "OverseerProgression.h"
#include "ArchiveSubsystem.h"
#include "Engine/Texture2D.h"
#include "Engine/GameViewportClient.h"
#include "Engine/GameInstance.h"
#include "ImageUtils.h"
#include "Misc/Paths.h"
#include "Kismet/GameplayStatics.h"

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Load the title art at runtime from the project Content folder (no editor import needed).
	const FString ImagePath = FPaths::ProjectContentDir() / TEXT("UI/TitleScreen.png");
	if (FPaths::FileExists(ImagePath))
	{
		TitleTexture = FImageUtils::ImportFileAsTexture2D(ImagePath);
	}

	ShowTitle();
}

void AMainMenuPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllWidgets();
	Super::EndPlay(EndPlayReason);
}

void AMainMenuPlayerController::ApplyUIInputMode()
{
	FInputModeUIOnly Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void AMainMenuPlayerController::RemoveAllWidgets()
{
	if (!GEngine || !GEngine->GameViewport) return;

	if (TitleWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(TitleWidget.ToSharedRef());
		TitleWidget.Reset();
	}
	if (HubWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(HubWidget.ToSharedRef());
		HubWidget.Reset();
	}
}

void AMainMenuPlayerController::ShowTitle()
{
	if (!GEngine || !GEngine->GameViewport) return;
	RemoveAllWidgets();

	SAssignNew(TitleWidget, SMainMenuWidget)
		.BackgroundTexture(TitleTexture)
		.OnStartGame(FSimpleDelegate::CreateUObject(this, &AMainMenuPlayerController::OnStartGame))
		.OnQuit(FSimpleDelegate::CreateUObject(this, &AMainMenuPlayerController::OnQuit));

	GEngine->GameViewport->AddViewportWidgetContent(TitleWidget.ToSharedRef(), 1000);
	ApplyUIInputMode();
}

void AMainMenuPlayerController::ShowHub()
{
	if (!GEngine || !GEngine->GameViewport) return;
	RemoveAllWidgets();

	UOverseerProgression* Progress = GetGameInstance() ? GetGameInstance()->GetSubsystem<UOverseerProgression>() : nullptr;
	UArchiveSubsystem* ArchiveSys = GetGameInstance() ? GetGameInstance()->GetSubsystem<UArchiveSubsystem>() : nullptr;

	SAssignNew(HubWidget, SSiteOverseerHub)
		.Progression(Progress)
		.Archive(ArchiveSys)
		.OnDeployBreach(FSimpleDelegate::CreateUObject(this, &AMainMenuPlayerController::OnDeployBreach))
		.OnBackToTitle(FSimpleDelegate::CreateUObject(this, &AMainMenuPlayerController::OnBackToTitle));

	GEngine->GameViewport->AddViewportWidgetContent(HubWidget.ToSharedRef(), 1000);
	ApplyUIInputMode();
}

void AMainMenuPlayerController::OnStartGame()
{
	ShowHub();
}

void AMainMenuPlayerController::OnBackToTitle()
{
	ShowTitle();
}

void AMainMenuPlayerController::OnDeployBreach()
{
	RemoveAllWidgets();
	const FString Level = UGameplayStatics::GetCurrentLevelName(this, true);
	// Re-open the current map with the FPS gameplay GameMode override.
	UGameplayStatics::OpenLevel(this, FName(*Level), true, TEXT("Game=/Script/Foundation.FoundationGameMode"));
}

void AMainMenuPlayerController::OnQuit()
{
	ConsoleCommand(TEXT("quit"));
}
