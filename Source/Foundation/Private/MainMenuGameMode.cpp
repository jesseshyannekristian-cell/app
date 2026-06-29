#include "MainMenuGameMode.h"
#include "MainMenuPlayerController.h"
#include "GameFramework/SpectatorPawn.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	PlayerControllerClass = AMainMenuPlayerController::StaticClass();
	DefaultPawnClass = ASpectatorPawn::StaticClass();
}
