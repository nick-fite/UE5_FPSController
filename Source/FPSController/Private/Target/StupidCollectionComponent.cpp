// Fill out your copyright notice in the Description page of Project Settings.


#include "Target/StupidCollectionComponent.h"

void UStupidCollectionComponent::SetEnableGravity(bool bGravityEnabled)
{
	BodyInstance.SetEnableGravity(bGravityEnabled);
	RecreatePhysicsState();
	UE_LOG(LogTemp, Warning, TEXT("setting gravity"));
}
