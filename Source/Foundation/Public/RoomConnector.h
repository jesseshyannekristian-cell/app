#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RoomConnector.generated.h"

/** A door opening on a room. Its forward (+X) axis points OUT of the room. */
UCLASS(ClassGroup = (Foundation), meta = (BlueprintSpawnableComponent))
class FOUNDATION_API URoomConnector : public USceneComponent
{
	GENERATED_BODY()

public:
	URoomConnector();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Connector")
	bool bUsed = false;
};
