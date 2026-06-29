#include "ContainmentTerminal.h"
#include "FoundationCharacter.h"
#include "FoundationGameMode.h"
#include "SCPEntityBase.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

AContainmentTerminal::AContainmentTerminal()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetRelativeScale3D(FVector(0.6f, 0.3f, 1.4f));
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		Mesh->SetStaticMesh(CubeFinder.Object);
	}
}

void AContainmentTerminal::Interact_Implementation(AFoundationCharacter* Interactor)
{
	if (!Interactor)
	{
		return;
	}

	ASCPEntityBase* Nearest = nullptr;
	float BestDist = ContainmentRange;
	for (TActorIterator<ASCPEntityBase> It(GetWorld()); It; ++It)
	{
		if (It->bContained) { continue; }
		const float D = FVector::Dist(GetActorLocation(), It->GetActorLocation());
		if (D < BestDist)
		{
			BestDist = D;
			Nearest = *It;
		}
	}

	if (!Nearest)
	{
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("No breached anomaly in range.")); }
		return;
	}

	if (Interactor->KeycardLevel < Nearest->ContainmentKeycardLevel)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange,
				FString::Printf(TEXT("%s [%s] requires Level %d clearance. Procedure: %s"),
					*Nearest->Designation,
					*UEnum::GetDisplayValueAsText(Nearest->ContainmentClass).ToString(),
					Nearest->ContainmentKeycardLevel,
					*Nearest->ContainmentProcedure));
		}
		return;
	}

	Nearest->Recontain();

	if (AFoundationGameMode* GM = Cast<AFoundationGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->NotifyRecontained();
	}
}

FString AContainmentTerminal::GetInteractPrompt_Implementation() const
{
	return TEXT("Use Containment Terminal");
}
