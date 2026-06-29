#include "FoundationGameMode.h"
#include "FoundationCharacter.h"
#include "FacilityGenerator.h"
#include "EncounterManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

AFoundationGameMode::AFoundationGameMode()
{
	DefaultPawnClass = AFoundationCharacter::StaticClass();
	GeneratorClass = AFacilityGenerator::StaticClass();
	EncounterManagerClass = AEncounterManager::StaticClass();
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

void AFoundationGameMode::HandlePlayerDeath()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("TERMINATED. Restarting..."));
	}

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AFoundationGameMode::RestartLevel, RestartDelay, false);
}

void AFoundationGameMode::HandlePlayerEscaped()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("CONTAINMENT BREACH SURVIVED. You escaped!"));
	}

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AFoundationGameMode::RestartLevel, RestartDelay, false);
}

void AFoundationGameMode::RestartLevel()
{
	const FName Current(*UGameplayStatics::GetCurrentLevelName(this, true));
	UGameplayStatics::OpenLevel(this, Current);
}
