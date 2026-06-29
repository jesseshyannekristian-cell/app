#include "DoorActor.h"
#include "FoundationCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ADoorActor::ADoorActor()
{
	PrimaryActorTick.bCanEverTick = true;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	SetRootComponent(DoorMesh);
	DoorMesh->SetRelativeScale3D(FVector(0.2f, 2.4f, 3.f));
	DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		DoorMesh->SetStaticMesh(CubeFinder.Object);
	}
}

void ADoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bOpen)
	{
		return;
	}

	const FVector Loc = GetActorLocation();
	const float TargetZ = ClosedZ + OpenHeight;
	if (Loc.Z < TargetZ)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, FMath::Min(OpenSpeed * DeltaTime, TargetZ - Loc.Z)));
	}
}

void ADoorActor::Interact_Implementation(AFoundationCharacter* Interactor)
{
	if (bOpen || !Interactor)
	{
		return;
	}

	if (Interactor->KeycardLevel >= RequiredKeycardLevel)
	{
		ClosedZ = GetActorLocation().Z;
		bOpen = true;
		DoorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Door unlocked."));
		}
	}
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange,
			FString::Printf(TEXT("Locked. Requires Level %d keycard."), RequiredKeycardLevel));
	}
}

FString ADoorActor::GetInteractPrompt_Implementation() const
{
	return FString::Printf(TEXT("Open Door (Level %d)"), RequiredKeycardLevel);
}
