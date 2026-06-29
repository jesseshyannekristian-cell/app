#include "SCP096.h"
#include "FoundationCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ASCP096::ASCP096()
{
	Designation = TEXT("SCP-096");
	ContainmentClass = EContainmentClass::Euclid;
	ContainmentProcedure = TEXT("Never view the face of SCP-096. Contain in a sealed cell with no observation windows.");
	ContainmentKeycardLevel = 4;
	MoveSpeed = 1400.f;
	KillDistance = 150.f;
	Health = 1500.f;

	if (BodyMesh)
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> Cyl(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
		if (Cyl.Succeeded()) { BodyMesh->SetStaticMesh(Cyl.Object); }
		BodyMesh->SetRelativeScale3D(FVector(0.65f, 0.65f, 2.1f)); // tall, gaunt humanoid
	}
}

bool ASCP096::CanMove(AFoundationCharacter* /*Player*/) const
{
	return bEnraged;
}

void ASCP096::Tick(float DeltaTime)
{
	DetectViewed();
	Super::Tick(DeltaTime);
}

void ASCP096::DetectViewed()
{
	if (bEnraged || bContained)
	{
		return;
	}

	AFoundationCharacter* Player = GetTargetPlayer();
	if (!Player || !Player->Camera)
	{
		return;
	}

	const FVector CamLoc = Player->Camera->GetComponentLocation();
	FVector ToSelf = (GetActorLocation() + FVector(0, 0, 90.f)) - CamLoc;
	const float Dist = ToSelf.Size();
	if (Dist < 1.f) { return; }
	ToSelf /= Dist;

	if (FVector::DotProduct(Player->Camera->GetForwardVector(), ToSelf) < ViewDotThreshold)
	{
		return;
	}

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Player);
	Params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, GetActorLocation() + FVector(0, 0, 90.f), ECC_Visibility, Params))
	{
		return;
	}

	bEnraged = true;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("SCP-096 has seen you. RUN."));
	}
}
