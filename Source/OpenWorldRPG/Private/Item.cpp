// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "DrawDebugHelpers.h"


// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	itemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = itemMesh;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	Avg<int32>(1, 3);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	runningTime += DeltaTime;

	float deltaZ = FMath::Sin(runningTime);

	AddActorWorldOffset(FVector(0.f, 0.f, deltaZ));
}

