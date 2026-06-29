#pragma once

#include "CoreMinimal.h"
#include "SCPEntityBase.h"
#include "SCP049.generated.h"

/**
 * SCP-049 — "The Plague Doctor". Walks deliberately toward personnel and is
 * lethal on contact (its anomalous "cure"). Euclid-class; uses the base pursue
 * and contact-kill behaviour with its own tuning.
 */
UCLASS()
class FOUNDATION_API ASCP049 : public ASCPEntityBase
{
	GENERATED_BODY()

public:
	ASCP049();
};
