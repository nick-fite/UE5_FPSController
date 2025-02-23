// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "FPSCharacter.generated.h"

class UCameraComponent;

UCLASS()
class AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	//Inputs
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* MappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* LookInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* MoveInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* JumpInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* SlideInputAction;
	
	//Camera
	UPROPERTY(VisibleDefaultsOnly, Category="View")
	class UCameraComponent* ViewCam;

	UPROPERTY(VisibleDefaultsOnly, Category="View")
	class USpringArmComponent* CameraBoom;

	FVector GetLookRightDirection() const;
	FVector GetLookForwardDirection() const;
	FVector GetMoveForwardDirection() const;

	void HandleLookInput(const FInputActionValue& InputActionValue);
	void HandleMoveInput(const FInputActionValue& InputActionValue);
	void HandleSlideInput(const FInputActionValue& InputActionValue);
	
	//slide
	UPROPERTY(VisibleDefaultsOnly, Category = "Slide")
	bool bIsSliding;

	UPROPERTY(EditDefaultsOnly, Category="Slide")
	float SlideTime = 0.5f;
	
	FTimerHandle SlideHandle; 
	
	void UnsetSlide() const;
};
