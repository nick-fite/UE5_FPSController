// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Target/Target.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 0.f;

	bUseControllerRotationYaw = false;

	ViewCam = CreateDefaultSubobject<UCameraComponent>("View Cam");
	ViewCam->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(1200.f);

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 3000;
	GetCharacterMovement()->MaxWalkSpeed = 2000;
	GetCharacterMovement()->MaxAcceleration = 6000;

	GunMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("Static Gun");
	GunMeshComp->SetupAttachment(ViewCam);
	SlideHandle = FTimerHandle();

	Arrow = CreateDefaultSubobject<UArrowComponent>("ShootPos");
	Arrow->SetupAttachment(GunMeshComp);
	
	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if(cameraManager)
	{
		cameraManager->ViewPitchMax = 359;
		cameraManager->ViewPitchMin = -359;
	}
	RotationTimeElapsed = 0.0f;
	bIsShooting = false;
	MoveDuration = 0.2f;
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	if(APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			subsys->RemoveMappingContext(MappingContext);
			subsys->AddMappingContext(MappingContext, 0);
		}
	}
		
		GetCharacterMovement()->MaxAcceleration = 6000;

	InitGunRotation = GunMeshComp->GetRelativeRotation();
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bReloading)
	{
		RotationTimeElapsed += DeltaTime;
		float Alpha = FMath::Clamp(RotationTimeElapsed/RotationDuration, 0.f, 1.f);
		int newRot = FMath::Lerp(InitGunRotation.Pitch, InitGunRotation.Pitch - 360, Alpha);
		GunMeshComp->SetRelativeRotation(FRotator(newRot, InitGunRotation.Yaw, InitGunRotation.Roll));

		if(RotationTimeElapsed >= RotationDuration)
		{
			bReloading = false;
			UE_LOG(LogTemp, Warning, TEXT("Done rot"));
			GunMeshComp->SetRelativeRotation(InitGunRotation);
			GetWorldTimerManager().ClearTimer(GunRotateHandle);
			bIsShooting = false;
		}
	}
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if(EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &AFPSCharacter::HandleLookInput);
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &AFPSCharacter::HandleMoveInput);
		EnhancedInputComponent->BindAction(SlideInputAction, ETriggerEvent::Triggered, this, &AFPSCharacter::HandleSlideInput);
		EnhancedInputComponent->BindAction(ShootInputAction, ETriggerEvent::Triggered, this, &AFPSCharacter::HandleShootInput);
		EnhancedInputComponent->BindAction(ReloadInputAction, ETriggerEvent::Triggered, this, &AFPSCharacter::HandleReloadInput);
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Jump);
	}
}

FVector AFPSCharacter::GetLookRightDirection() const
{
	return ViewCam->GetRightVector();
}

FVector AFPSCharacter::GetLookForwardDirection() const
{
	return ViewCam->GetForwardVector();
}

FVector AFPSCharacter::GetMoveForwardDirection() const
{
	return FVector::CrossProduct(GetLookRightDirection(), FVector::UpVector);
}

void AFPSCharacter::HandleLookInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputVal = InputActionValue.Get<FVector2D>();
	AddControllerYawInput(InputVal.X);
	AddControllerPitchInput(InputVal.Y);
}

void AFPSCharacter::HandleMoveInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputVal = InputActionValue.Get<FVector2D>();
	if(InputVal.Length() < 0)
	{
		return;
	}
	InputVal.Normalize();
	AddMovementInput(GetLookRightDirection() * InputVal.X + GetMoveForwardDirection() * InputVal.Y);
}

void AFPSCharacter::HandleSlideInput(const FInputActionValue& InputActionValue)
{
	bool InputVal = InputActionValue.Get<bool>();

	if(!bIsSliding && InputVal)
	{
		GetCharacterMovement()->bWantsToCrouch = true;
		GetCharacterMovement()->SetCrouchedHalfHeight(5);
		GetCharacterMovement()->Crouch();

		GetWorldTimerManager().SetTimer(SlideHandle, this, &AFPSCharacter::UnsetSlide, SlideTime, false);
		bIsSliding = true;
	}
	else if (!InputVal)
	{
		UnsetSlide();
		bIsSliding = false;
	}
}

void AFPSCharacter::HandleShootInput(const FInputActionValue& InputActionValue)
{
	if(bReloading || bIsShooting) return; 
	FVector startLoc = GetActorLocation();
	FVector endLoc = startLoc + (ViewCam->GetForwardVector() * 100000.f);

	FVector camLoc = ViewCam->GetComponentLocation();
	FRotator camRot = ViewCam->GetComponentRotation();
	
	GetController()->GetPlayerViewPoint(camLoc, camRot);

	
	FHitResult Hit;
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		startLoc,
		endLoc,
		ECC_Visibility
	);

	if(bHit)
	{
		ATarget* target = Cast<ATarget>( Hit.GetActor());
		if(target)
		{
			UE_LOG(LogTemp, Warning, TEXT("hit: %s"), *Hit.GetActor()->GetName());
			target->ExplodeMesh(this);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TargetDestroyParticleSystem, target->GetActorLocation() );
		}
	}
	UParticleSystemComponent* gunFlash = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlashParticleSystem, Arrow->GetComponentLocation(), FRotator::ZeroRotator, FVector(.5f));
	gunFlash->SetupAttachment(RootComponent);
	StartCameraShake();
	bIsShooting = true;
	GetWorldTimerManager().SetTimer(GunRotateHandle, this, &AFPSCharacter::StartGunRotation, MoveDuration, false);
}

void AFPSCharacter::HandleReloadInput(const FInputActionValue& InputActionValue)
{
	StartGunRotation();
}

void AFPSCharacter::UnsetSlide()
{
	GetWorldTimerManager().ClearTimer(SlideHandle);
	GetCharacterMovement()->bWantsToCrouch = false;
	GetCharacterMovement()->SetCrouchedHalfHeight(40);
	GetCharacterMovement()->UnCrouch();
}

void AFPSCharacter::StartGunRotation()
{
	RotationTimeElapsed = 0.f;
	bReloading = true;
}

void AFPSCharacter::StartCameraShake_Implementation()
{
}
