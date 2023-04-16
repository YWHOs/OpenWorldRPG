// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	itemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = itemMesh;

	sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	sphere->SetupAttachment(GetRootComponent());
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
	const FString actorName = OtherActor->GetName();

	if (GEngine) 
	{
		GEngine->AddOnScreenDebugMessage(1, 30.f, FColor::Red, actorName);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const FString actorName = FString("Ending: ") + OtherActor->GetName();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 30.f, FColor::Blue, actorName);
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	runningTime += DeltaTime;

	//float deltaZ = FMath::Sin(runningTime);
	//AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
}

