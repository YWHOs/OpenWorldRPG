// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "PickupInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	itemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	itemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = itemMesh;

	sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	sphere->SetupAttachment(GetRootComponent());

	emberEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	emberEffect->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
	Avg<int32>(1, 3);
}

float AItem::TransformedSin()
{
	return amplitude * FMath::Sin(runningTime * timeConstant);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* hitInterface = Cast<IPickupInterface>(OtherActor);
	if (hitInterface)
	{
		hitInterface->SetOverlapItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPickupInterface* hitInterface = Cast<IPickupInterface>(OtherActor);
	if (hitInterface)
	{
		hitInterface->SetOverlapItem(nullptr);
	}
}
void AItem::SpawnPickupSystem()
{
	if (pickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, pickupEffect, GetActorLocation());
	}
}
void AItem::SpawnPickupSound()
{
	if (pickupSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, pickupSound, GetActorLocation());
	}
}
// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	runningTime += DeltaTime;

	if (itemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
	

	//float deltaZ = FMath::Sin(runningTime);
	//AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
}

