#include "FoundationCharacter.h"
#include "SanityComponent.h"
#include "InteractableInterface.h"
#include "FoundationGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

AFoundationCharacter::AFoundationCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(40.f, 90.f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(0.f, 0.f, 70.f));
	Camera->bUsePawnControlRotation = true;

	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	Flashlight->SetupAttachment(Camera);
	Flashlight->SetIntensity(8000.f);
	Flashlight->SetOuterConeAngle(32.f);
	Flashlight->SetAttenuationRadius(2500.f);
	Flashlight->SetVisibility(false);

	Sanity = CreateDefaultSubobject<USanityComponent>(TEXT("Sanity"));

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AFoundationCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFoundationCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFoundationCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AFoundationCharacter::DoInteract);
	PlayerInputComponent->BindAction("Flashlight", IE_Pressed, this, &AFoundationCharacter::ToggleFlashlight);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AFoundationCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AFoundationCharacter::StopSprint);
}

void AFoundationCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.f)
	{
		const FRotator Yaw(0.f, Controller->GetControlRotation().Yaw, 0.f);
		AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::X), Value);
	}
}

void AFoundationCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.f)
	{
		const FRotator Yaw(0.f, Controller->GetControlRotation().Yaw, 0.f);
		AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y), Value);
	}
}

void AFoundationCharacter::DoInteract()
{
	if (!Camera)
	{
		return;
	}

	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * InteractDistance;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
		{
			IInteractableInterface::Execute_Interact(HitActor, this);
		}
	}
}

void AFoundationCharacter::ToggleFlashlight()
{
	if (Flashlight)
	{
		Flashlight->SetVisibility(!Flashlight->IsVisible());
	}
}

void AFoundationCharacter::StartSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AFoundationCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AFoundationCharacter::GiveKeycard(int32 Level)
{
	KeycardLevel = FMath::Max(KeycardLevel, Level);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
			FString::Printf(TEXT("Picked up Level %d keycard"), Level));
	}
}

bool AFoundationCharacter::IsBlinking() const
{
	return Sanity ? Sanity->bIsBlinking : false;
}

void AFoundationCharacter::KillPlayer()
{
	if (bDead)
	{
		return;
	}
	bDead = true;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	if (AFoundationGameMode* GM = Cast<AFoundationGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->HandlePlayerDeath();
	}
}

float AFoundationCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bDead)
	{
		return 0.f;
	}

	Health -= DamageAmount;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,
			FString::Printf(TEXT("Health: %.0f"), FMath::Max(Health, 0.f)));
	}

	if (Health <= 0.f)
	{
		KillPlayer();
	}
	return Applied;
}
