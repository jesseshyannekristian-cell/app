#include "SCP16829.h"
#include "FoundationCharacter.h"
#include "SanityComponent.h"
#include "RoomBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "EngineUtils.h"

ASCP16829::ASCP16829()
{
	Designation = TEXT("SCP-16829");
	ContainmentClass = EContainmentClass::Euclid;
	ContainmentProcedure = TEXT("Anchored within a Scranton Reality Anchor field; temporal-loop stabilisation prevents dimensional egress. The screen may not be viewed by unsupervised personnel.");
	ContainmentKeycardLevel = 4;

	// Stationary anomaly — no pursuit, no contact-kill.
	MoveSpeed = 0.f;
	KillDistance = 0.f;
	Health = 1500.f;

	if (BodyMesh)
	{
		BodyMesh->SetRelativeScale3D(FVector(0.9f, 0.6f, 0.7f)); // boxy CRT television
		BodyMesh->SetCollisionProfileName(TEXT("BlockAll"));     // so the interact line trace hits it
	}

	ScreenGlow = CreateDefaultSubobject<UPointLightComponent>(TEXT("ScreenGlow"));
	ScreenGlow->SetupAttachment(RootComponent);
	ScreenGlow->SetLightColor(FLinearColor(0.20f, 0.60f, 1.0f));
	ScreenGlow->SetIntensity(3000.f);
	ScreenGlow->SetAttenuationRadius(700.f);
}

void ASCP16829::Tick(float DeltaTime)
{
	// Intentionally NOT calling Super::Tick — SCP-16829 does not pursue or kill on contact.
	if (bContained)
	{
		return;
	}

	AFoundationCharacter* Player = GetTargetPlayer();
	if (!Player)
	{
		return;
	}

	const float Dist = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
	const bool bInField = Dist <= LoopFieldRadius;

	if (bInField)
	{
		if (!bPlayerInField)
		{
			bPlayerInField = true;
			LoopAnchor = Player->GetActorLocation();
			LoopTimer = 0.f;
		}

		if (Player->Sanity)
		{
			Player->Sanity->DrainSanity(ProximitySanityDrain * DeltaTime);
		}
		Exposure = FMath::Min(Exposure + DeltaTime * 2.f, 100.f);

		if (Exposure >= ExposureCancerThreshold)
		{
			Player->TakeDamage(CancerDamagePerSec * DeltaTime, FDamageEvent(), nullptr, this);
		}

		LoopTimer += DeltaTime;
		if (LoopTimer >= LoopInterval)
		{
			LoopTimer = 0.f;
			Player->SetActorLocation(LoopAnchor, false, nullptr, ETeleportType::TeleportPhysics);
			if (Player->Sanity)
			{
				Player->Sanity->DrainSanity(15.f);
			}
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan, TEXT("SCP-16829: the timeline loops. You are pulled back to where you began."));
			}
		}
	}
	else
	{
		bPlayerInField = false;
		Exposure = FMath::Max(Exposure - DeltaTime, 0.f);
	}
}

void ASCP16829::Interact_Implementation(AFoundationCharacter* Interactor)
{
	if (!Interactor || bContained)
	{
		return;
	}

	FVector Dest;
	if (FindRandomRoomLocation(Dest))
	{
		Interactor->SetActorLocation(Dest, false, nullptr, ETeleportType::TeleportPhysics);
		ApplyExposure(Interactor, ExposurePerUse);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan, TEXT("You step into Dimension Chemosic... and emerge elsewhere in the facility."));
		}
	}
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("SCP-16829 flickers, but the destination fails to resolve."));
	}
}

FString ASCP16829::GetInteractPrompt_Implementation() const
{
	return TEXT("Enter SCP-16829 (Dimension Chemosic)");
}

bool ASCP16829::FindRandomRoomLocation(FVector& OutLocation) const
{
	TArray<FVector> Locations;
	for (TActorIterator<ARoomBase> It(GetWorld()); It; ++It)
	{
		const FVector Loc = It->GetActorLocation() + FVector(0.f, 0.f, 120.f);
		if (FVector::Dist(Loc, GetActorLocation()) > 800.f) // avoid arriving on top of the TV
		{
			Locations.Add(Loc);
		}
	}
	if (Locations.Num() == 0)
	{
		return false;
	}
	OutLocation = Locations[FMath::RandRange(0, Locations.Num() - 1)];
	return true;
}

void ASCP16829::ApplyExposure(AFoundationCharacter* Player, float Amount)
{
	Exposure = FMath::Min(Exposure + Amount, 100.f);
	if (Player && Player->Sanity)
	{
		Player->Sanity->DrainSanity(Amount * 0.5f);
	}
	if (Exposure >= ExposureCancerThreshold && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("WARNING: SCP-16829 exposure critical — cellular degradation risk (cf. Agent CHEMOSIC)."));
	}
}
