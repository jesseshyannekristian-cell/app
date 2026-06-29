#include "SCP049.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ASCP049::ASCP049()
{
	Designation = TEXT("SCP-049");
	ContainmentClass = EContainmentClass::Euclid;
	ContainmentProcedure = TEXT("Standard humanoid containment cell. Physical contact is strictly prohibited — the subject's touch is lethal. Re-contain via a Level 3 terminal.");
	ContainmentKeycardLevel = 3;
	MoveSpeed = 200.f;
	KillDistance = 140.f;
	Health = 900.f;

	if (BodyMesh)
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> Cyl(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
		if (Cyl.Succeeded()) { BodyMesh->SetStaticMesh(Cyl.Object); }
		BodyMesh->SetRelativeScale3D(FVector(0.7f, 0.7f, 1.95f)); // robed plague doctor
	}
}
