#include "FoundationGameMode.h"
#include "FoundationCharacter.h"
#include "FacilityGenerator.h"
#include "EncounterManager.h"
#include "OverseerProgression.h"
#include "FoundationHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

static UOverseerProgression* GetProgression(const UObject* WorldContext)
{
	if (const AGameModeBase* GM = Cast<AGameModeBase>(WorldContext))
	{
		if (UGameInstance* GI = GM->GetGameInstance())
		{
			return GI->GetSubsystem<UOverseerProgression>();
		}
	}
	return nullptr;
}

AFoundationGameMode::AFoundationGameMode()
{
	DefaultPawnClass = AFoundationCharacter::StaticClass();
	GeneratorClass = AFacilityGenerator::StaticClass();
	EncounterManagerClass = AEncounterManager::StaticClass();
	HUDClass = AFoundationHUD::StaticClass();
}

void AFoundationGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GeneratorClass && GetWorld())
	{
		Generator = GetWorld()->SpawnActor<AFacilityGenerator>(GeneratorClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (Generator)
		{
			Generator->Generate();

			// Drop the player into the generated start room.
			if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
			{
				Pawn->SetActorLocation(Generator->GetStartLocation());
			}
		}
	}

	if (EncounterManagerClass && GetWorld())
	{
		EncounterMgr = GetWorld()->SpawnActor<AEncounterManager>(EncounterManagerClass, FVector::ZeroVector, FRotator::ZeroRotator);
	}
}

AActor* AFoundationGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// The facility is built around the origin; spawn there if no PlayerStart exists.
	if (AActor* Start = Super::ChoosePlayerStart_Implementation(Player))
	{
		return Start;
	}
	return nullptr;
}

void AFoundationGameMode::NotifyRecontained()
{
	SCPsRecontainedThisRun++;
}

void AFoundationGameMode::HandlePlayerDeath()
{
	if (UOverseerProgression* Prog = GetProgression(this))
	{
		Prog->RecordBreachResult(false, 0);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("TERMINATED. Returning to Site Overseer..."));
	}

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AFoundationGameMode::RestartLevel, RestartDelay, false);
}

void AFoundationGameMode::HandlePlayerEscaped()
{
	if (UOverseerProgression* Prog = GetProgression(this))
	{
		Prog->RecordBreachResult(true, SCPsRecontainedThisRun);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("CONTAINMENT BREACH SURVIVED. You escaped!"));
	}

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AFoundationGameMode::RestartLevel, RestartDelay, false);
}

void AFoundationGameMode::RestartLevel()
{
	// Return to the Site Overseer hub (default GameMode) after a breach ends.
	const FName Current(*UGameplayStatics::GetCurrentLevelName(this, true));
	UGameplayStatics::OpenLevel(this, Current);
}
