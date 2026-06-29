#include "SCP682.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"

ASCP682::ASCP682()
{
	Designation = TEXT("SCP-682");
	ContainmentClass = EContainmentClass::Keter;
	ContainmentProcedure = TEXT("Submerge in hydrochloric acid. Re-containment requires Level 5 authorization; near-impossible to neutralize.");
	ContainmentKeycardLevel = 5;
	MoveSpeed = 400.f;
	KillDistance = 170.f;
	Health = 8000.f;

	if (Capsule)
	{
		Capsule->InitCapsuleSize(70.f, 110.f);
	}
	if (BodyMesh)
	{
		BodyMesh->SetRelativeScale3D(FVector(1.6f, 1.0f, 1.2f));
	}
}
