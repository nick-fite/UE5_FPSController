// Fill out your copyright notice in the Description page of Project Settings.


#include "Target/Target.h"

#include "Engine/DamageEvents.h"

// Sets default values
ATarget::ATarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CollectionComp = CreateDefaultSubobject<UStupidCollectionComponent>("Collection Comp");
	RootComponent = CollectionComp;
	
}

// Called when the game starts or when spawned
void ATarget::BeginPlay()
{
	Super::BeginPlay();
	//DestroyMesh();
}

// Called every frame
void ATarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATarget::WaitThenBreak()
{
	FVector spawnLoc = GetActorLocation();
	FRotator spawnRot = GetActorRotation();
	GetWorld()->SpawnActor<AActor>(masterField, spawnLoc, spawnRot);
}

void ATarget::ExplodeMesh(AActor* player)
{
	UE_LOG(LogTemp, Warning, TEXT("This is exploding"));
	CollectionComp->SetEnableGravity(true);
	FTimerHandle handle = FTimerHandle();
	GetWorldTimerManager().SetTimer(handle, this, &ATarget::WaitThenBreak, .01f, false);
	
	//CollectionComp->AddImpulse(FVector(10,10,10));
	//FDamageEvent test = FDamageEvent();
	//CollectionComp->ReceiveComponentDamage(10, test, player->GetInstigatorController(), player);
	//CollectionComp->getcomponent
	//GetWorld()->SpawnActor(MasterFieldToSpawn);
	//AFieldSystemActor* test = GetWorld()->SpawnActor<AFieldSystemActor>(AFieldSystemActor::StaticClass());
	
}



