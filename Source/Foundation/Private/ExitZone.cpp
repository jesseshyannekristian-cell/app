#include "ExitZone.h"
#include "FoundationCharacter.h"
#include "FoundationGameMode.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AExitZone::AExitZone()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);
	Trigger->SetBoxExtent(FVector(120.f, 120.f, 150.f));
	Trigger->SetCollisionProfileName(TEXT("Trigger"));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AExitZone::OnOverlap);

	Marker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Marker"));
	Marker->SetupAttachment(Trigger);
	Marker->SetRelativeScale3D(FVector(2.4f, 2.4f, 0.1f));
	Marker->SetRelativeLocation(FVector(0.f, 0.f, -140.f));
	Marker->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		Marker->SetStaticMesh(CubeFinder.Object);
	}
}

void AExitZone::OnOverlap(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*Sweep*/)
{
	AFoundationCharacter* Player = Cast<AFoundationCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	if (Player->KeycardLevel < RequiredKeycardLevel)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange,
				FString::Printf(TEXT("Exit sealed. Need a Level %d keycard."), RequiredKeycardLevel));
		}
		return;
	}

	if (AFoundationGameMode* GM = Cast<AFoundationGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->HandlePlayerEscaped();
	}
}
