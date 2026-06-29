#include "FoundationHUD.h"
#include "SFoundationHUDWidget.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Engine.h"

void AFoundationHUD::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine && GEngine->GameViewport)
	{
		SAssignNew(HUDWidget, SFoundationHUDWidget).OwnerHUD(this);
		GEngine->GameViewport->AddViewportWidgetContent(HUDWidget.ToSharedRef(), 0);
	}
}

void AFoundationHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HUDWidget.IsValid() && GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(HUDWidget.ToSharedRef());
		HUDWidget.Reset();
	}
	Super::EndPlay(EndPlayReason);
}
