// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "StupidCollectionComponent.generated.h"

/**
 * 
 */
UCLASS()
class UStupidCollectionComponent : public UGeometryCollectionComponent
{
	GENERATED_BODY()

public:
	virtual void SetEnableGravity(bool bGravityEnabled) override;
};
