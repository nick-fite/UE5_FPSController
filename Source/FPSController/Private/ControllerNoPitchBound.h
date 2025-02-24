// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ControllerNoPitchBound.generated.h"

/**
 * 
 */
UCLASS()
class AControllerNoPitchBound : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void AddPitchInput(float Val) override;	
};
