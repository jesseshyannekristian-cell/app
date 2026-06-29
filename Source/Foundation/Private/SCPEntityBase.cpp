#include "SCPEntityBase.h"
#include "FoundationCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

ASCPEntityBase::ASCPEntityBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->InitCapsuleSize(45.f, 95.f);
	Capsule->SetCollisionProfileName(TEXT("Pawn"));
	SetRootComponent(Capsule);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(Capsule);
	BodyMesh->SetRelativeScale3D(FVector(0.7f, 0.7f, 1.9f));
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeFinder.Object);
	}
}

AFoundationCharacter* ASCPEntityBase::GetTargetPlayer() const
{
	return Cast<AFoundationCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}

bool ASCPEntityBase::CanMove(AFoundationCharacter* /*Player*/) const
{
	return true;
}

void ASCPEntityBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bContained)
	{
		return;
	}

	AFoundationCharacter* Player = GetTargetPlayer();
	if (!Player)
	{
		return;
	}

	if (CanMove(Player))
	{
		MoveTowardPlayer(DeltaTime, Player);
	}

	TryKill(Player);
}

void ASCPEntityBase::MoveTowardPlayer(float DeltaTime, AFoundationCharacter* Player)
{
	FVector ToPlayer = Player->GetActorLocation() - GetActorLocation();
	ToPlayer.Z = 0.f;
	const float Dist = ToPlayer.Size();
	if (Dist < 1.f)
	{
		return;
	}

	const FVector Dir = ToPlayer / Dist;
	AddActorWorldOffset(Dir * MoveSpeed * DeltaTime, true);

	const FRotator Face = FRotationMatrix::MakeFromX(Dir).Rotator();
	SetActorRotation(FRotator(0.f, Face.Yaw, 0.f));
}

void ASCPEntityBase::TryKill(AFoundationCharacter* Player)
{
	if (FVector::Dist(GetActorLocation(), Player->GetActorLocation()) <= KillDistance)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red,
				FString::Printf(TEXT("%s caught you."), *Designation));
		}
		Player->KillPlayer();
	}
}

float ASCPEntityBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= DamageAmount;
	if (Health <= 0.f && !bContained)
	{
		// Gunfire alone can't kill most anomalies; it merely suppresses/neutralises weaker ones.
		Recontain();
	}
	return Applied;
}

void ASCPEntityBase::Recontain()
{
	if (bContained)
	{
		return;
	}
	bContained = true;
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Green,
			FString::Printf(TEXT("%s re-contained."), *Designation));
	}
}
