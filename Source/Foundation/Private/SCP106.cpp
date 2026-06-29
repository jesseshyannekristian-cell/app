#include "SCP106.h"
#include "FoundationCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ASCP106::ASCP106()
{
	Designation = TEXT("SCP-106");
	ContainmentClass = EContainmentClass::Keter;
	ContainmentProcedure = TEXT("Lure into the femur breaker / containment chamber. Re-contain via a Level 4 terminal.");
	ContainmentKeycardLevel = 4;
	MoveSpeed = 150.f;
	KillDistance = 150.f;
	Health = 2500.f;

	if (BodyMesh)
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> Cyl(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
		if (Cyl.Succeeded()) { BodyMesh->SetStaticMesh(Cyl.Object); }
		BodyMesh->SetRelativeScale3D(FVector(0.95f, 0.95f, 1.7f)); // hunched old man
	}
}

void ASCP106::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bContained)
	{
		return;
	}

	TeleportTimer += DeltaTime;
	if (TeleportTimer >= TeleportInterval)
	{
		TeleportTimer = 0.f;
		TryPhase();
	}
}

void ASCP106::TryPhase()
{
	AFoundationCharacter* Player = GetTargetPlayer();
	if (!Player)
	{
		return;
	}

	const float Dist = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
	if (Dist < TeleportMinRange)
	{
		return;
	}

	// Phase to just behind the player.
	const FVector Behind = Player->GetActorLocation() - Player->GetActorForwardVector() * 400.f + FVector(0, 0, 50.f);
	SetActorLocation(Behind, false, nullptr, ETeleportType::TeleportPhysics);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, TEXT("SCP-106 phases through the walls..."));
	}
}
