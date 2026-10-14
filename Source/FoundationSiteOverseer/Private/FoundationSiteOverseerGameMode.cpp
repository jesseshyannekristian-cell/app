#include "FoundationSiteOverseerGameMode.h"

#include "FoundationSiteOverseerCharacter.h"

AFoundationSiteOverseerGameMode::AFoundationSiteOverseerGameMode()
{
	DefaultPawnClass = AFoundationSiteOverseerCharacter::StaticClass();
}