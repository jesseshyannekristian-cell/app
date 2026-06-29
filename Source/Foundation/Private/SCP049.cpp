#include "SCP049.h"

ASCP049::ASCP049()
{
	Designation = TEXT("SCP-049");
	ContainmentClass = EContainmentClass::Euclid;
	ContainmentProcedure = TEXT("Standard humanoid containment cell. Physical contact is strictly prohibited — the subject's touch is lethal. Re-contain via a Level 3 terminal.");
	ContainmentKeycardLevel = 3;
	MoveSpeed = 200.f;
	KillDistance = 140.f;
	Health = 900.f;
}
