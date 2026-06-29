#include "KeycardPickup.h"
#include "FoundationCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AKeycardPickup::AKeycardPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetRelativeScale3D(FVector(0.3f, 0.05f, 0.45f));
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		Mesh->SetStaticMesh(CubeFinder.Object);
	}
}

void AKeycardPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddActorLocalRotation(FRotator(0.f, 60.f * DeltaTime, 0.f));
}

void AKeycardPickup::Interact_Implementation(AFoundationCharacter* Interactor)
{
	if (Interactor)
	{
		Interactor->GiveKeycard(KeycardLevel);
		Destroy();
	}
}

FString AKeycardPickup::GetInteractPrompt_Implementation() const
{
	return FString::Printf(TEXT("Take Level %d Keycard"), KeycardLevel);
}
