// Fill out your copyright notice in the Description page of Project Settings.


#include "ControllerNoPitchBound.h"

void AControllerNoPitchBound::AddPitchInput(float Val)
{
	RotationInput.Pitch += Val;
}
