// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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
	
	SlideHandle = 
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
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	AddControllerPitchInput(-InputVal.Y);
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
		UE_LOG(LogTemp, Warning, TEXT("Setting slide %hhd"), InputVal);
		GetCharacterMovement()->bWantsToCrouch = true;
		GetCharacterMovement()->MaxWalkSpeedCrouched = 3000;
		GetCharacterMovement()->MaxAcceleration = 6000;
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

void AFPSCharacter::UnsetSlide() const
{
	UE_LOG(LogTemp, Warning, TEXT("Unsetting slide"));
	GetWorldTimerManager().ClearTimer(SlideHandle);
	GetCharacterMovement()->bWantsToCrouch = false;
	GetCharacterMovement()->MaxWalkSpeed = 600;
	GetCharacterMovement()->SetCrouchedHalfHeight(40);
	GetCharacterMovement()->UnCrouch();
}
