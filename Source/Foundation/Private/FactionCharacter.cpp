#include "FactionCharacter.h"
#include "FoundationCharacter.h"
#include "SCPEntityBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

AFactionCharacter::AFactionCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->InitCapsuleSize(40.f, 90.f);
	Capsule->SetCollisionProfileName(TEXT("Pawn"));
	SetRootComponent(Capsule);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(Capsule);
	BodyMesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 1.8f));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeFinder.Object);
	}
}

void AFactionCharacter::BeginPlay()
{
	Super::BeginPlay();
	ApplyTint();
	RoamDir = FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f).Vector();
}

void AFactionCharacter::ApplyTint()
{
	if (BodyMesh)
	{
		if (UMaterialInstanceDynamic* MID = BodyMesh->CreateAndSetMaterialInstanceDynamic(0))
		{
			MID->SetVectorParameterValue(TEXT("Color"), UFactionLibrary::GetFactionColor(Faction));
		}
	}
}

EFaction AFactionCharacter::GetActorFaction(AActor* Actor) const
{
	if (Cast<AFoundationCharacter>(Actor)) { return EFaction::Foundation; }
	if (const AFactionCharacter* FC = Cast<AFactionCharacter>(Actor)) { return FC->Faction; }
	if (Cast<ASCPEntityBase>(Actor)) { return EFaction::Anomaly; }
	return EFaction::Civilian;
}

AActor* AFactionCharacter::FindHostileTarget()
{
	AActor* Best = nullptr;
	float BestDist = SenseRadius;
	const FVector MyLoc = GetActorLocation();

	auto Consider = [&](AActor* Other)
	{
		if (!Other || Other == this) { return; }
		if (UFactionLibrary::GetAttitude(Faction, GetActorFaction(Other)) != EAttitude::Hostile) { return; }
		const float D = FVector::Dist(MyLoc, Other->GetActorLocation());
		if (D < BestDist)
		{
			BestDist = D;
			Best = Other;
		}
	};

	Consider(UGameplayStatics::GetPlayerPawn(this, 0));

	for (TActorIterator<AFactionCharacter> It(GetWorld()); It; ++It) { Consider(*It); }
	for (TActorIterator<ASCPEntityBase> It(GetWorld()); It; ++It)
	{
		if (!It->bContained) { Consider(*It); }
	}

	return Best;
}

void AFactionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireCooldown -= DeltaTime;
	RetargetCooldown -= DeltaTime;

	if (RetargetCooldown <= 0.f || !CurrentTarget || !IsValid(CurrentTarget))
	{
		RetargetCooldown = 0.6f;
		CurrentTarget = FindHostileTarget();
	}

	if (CurrentTarget && IsValid(CurrentTarget))
	{
		EngageTarget(DeltaTime);
	}
	else
	{
		Roam(DeltaTime);
	}
}

void AFactionCharacter::EngageTarget(float DeltaTime)
{
	const FVector ToTarget = CurrentTarget->GetActorLocation() - GetActorLocation();
	FVector Flat = ToTarget; Flat.Z = 0.f;
	const float Dist = Flat.Size();
	if (Dist > 1.f)
	{
		SetActorRotation(FRotator(0.f, Flat.Rotation().Yaw, 0.f));
	}

	if (Dist > AttackRange)
	{
		const FVector Dir = Flat.GetSafeNormal();
		AddActorWorldOffset(Dir * MoveSpeed * DeltaTime, true);
	}
	else if (FireCooldown <= 0.f && HasLineOfSight(CurrentTarget))
	{
		FireAt(CurrentTarget);
		FireCooldown = FireInterval;
	}
}

void AFactionCharacter::Roam(float DeltaTime)
{
	RoamCooldown -= DeltaTime;
	if (RoamCooldown <= 0.f)
	{
		RoamCooldown = FMath::FRandRange(2.f, 4.f);
		RoamDir = FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f).Vector();
	}

	FHitResult Hit;
	if (!AddActorWorldOffset(RoamDir * (MoveSpeed * 0.4f) * DeltaTime, true, &Hit) || Hit.bBlockingHit)
	{
		RoamDir = RoamDir.RotateAngleAxis(FMath::FRandRange(90.f, 180.f), FVector::UpVector);
	}
	SetActorRotation(FRotator(0.f, RoamDir.Rotation().Yaw, 0.f));
}

bool AFactionCharacter::HasLineOfSight(AActor* Target) const
{
	if (!Target) { return false; }
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Target);
	const FVector Start = GetActorLocation() + FVector(0, 0, 60.f);
	const FVector End = Target->GetActorLocation() + FVector(0, 0, 40.f);
	return !GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
}

void AFactionCharacter::FireAt(AActor* Target)
{
	UGameplayStatics::ApplyDamage(Target, Damage, GetInstigatorController(), this, nullptr);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::White,
			FString::Printf(TEXT("%s [%s] fires!"), *OperativeName, *UFactionLibrary::GetFactionName(Faction)));
	}
}

float AFactionCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= DamageAmount;

	// Retaliate against whoever shot us.
	if (DamageCauser && DamageCauser != this)
	{
		CurrentTarget = DamageCauser;
		RetargetCooldown = 2.f;
	}

	if (Health <= 0.f)
	{
		Destroy();
	}
	return Applied;
}
