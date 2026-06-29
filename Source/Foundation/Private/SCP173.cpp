#include "SCP173.h"
#include "FoundationCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

ASCP173::ASCP173()
{
	Designation = TEXT("SCP-173");
	MoveSpeed = 1100.f;
	KillDistance = 140.f;
}

bool ASCP173::CanMove(AFoundationCharacter* Player) const
{
	// Free to move whenever it is NOT being observed.
	return !IsObservedBy(Player);
}

bool ASCP173::IsObservedBy(AFoundationCharacter* Player) const
{
	if (!Player || !Player->Camera)
	{
		return false;
	}

	// Blinking eyes can't observe.
	if (Player->IsBlinking())
	{
		return false;
	}

	const FVector CamLoc = Player->Camera->GetComponentLocation();
	const FVector CamFwd = Player->Camera->GetForwardVector();

	FVector ToSelf = GetActorLocation() - CamLoc;
	const float Dist = ToSelf.Size();
	if (Dist < 1.f)
	{
		return true;
	}
	ToSelf /= Dist;

	// Must be within the view cone.
	if (FVector::DotProduct(CamFwd, ToSelf) < ViewHalfAngleCos)
	{
		return false;
	}

	// Must have a clear line of sight.
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Player);
	Params.AddIgnoredActor(this);

	const FVector Target = GetActorLocation() + FVector(0, 0, 50.f);
	if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, Target, ECC_Visibility, Params))
	{
		return false; // something blocks the view
	}

	return true;
}
