// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StupidCollectionComponent.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Target.generated.h"


class AFieldSystemActor;

UCLASS()
class ATarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	//USkeletalMesh* Mesh;

	UPROPERTY(VisibleAnywhere)
	UStupidCollectionComponent* CollectionComp;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> masterField;

	
	FTimerHandle BreakHandle;
	void WaitThenBreak();
public:	
	UFUNCTION()
	void ExplodeMesh(AActor* player);
};
