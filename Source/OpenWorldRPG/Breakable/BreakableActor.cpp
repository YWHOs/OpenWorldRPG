// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "../Treasure.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABreakableActor::ABreakableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	geometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(geometryCollection);

	geometryCollection->SetGenerateOverlapEvents(true);
	geometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	geometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	capsule->SetupAttachment(GetRootComponent());
	capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

}

// Called when the game starts or when spawned
void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& _point)
{
	UWorld* world = GetWorld();
	if (world && treasureClasses.Num() > 0)
	{
		FVector location = GetActorLocation();
		location.Z += 75.f;

		const int32 selection = FMath::RandRange(0, treasureClasses.Num() - 1);
		world->SpawnActor<ATreasure>(treasureClasses[selection], location, GetActorRotation());
	}
	//GetWorld()->SpawnActor();
}

