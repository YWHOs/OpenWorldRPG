// Fill out your copyright notice in the Description page of Project Settings.


#include "Soul.h"
#include "PickupInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const double locationZ = GetActorLocation().Z;
	if (locationZ > desiredZ)
	{
		const FVector deltaLocation = FVector(0.f, 0.f, driftRate * DeltaTime);
		AddActorWorldOffset(deltaLocation);
	}
}
void ASoul::BeginPlay()
{
	Super::BeginPlay();

	const FVector start = GetActorLocation();
	const FVector end = start - FVector(0.f, 0.f, 2000.f);

	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	objectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(GetOwner());
	FHitResult hitResult;

	UKismetSystemLibrary::LineTraceSingleForObjects(this, start, end, objectTypes, false, actorsToIgnore, EDrawDebugTrace::None, hitResult, true);
	desiredZ = hitResult.ImpactPoint.Z;
}
void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* hitInterface = Cast<IPickupInterface>(OtherActor);
	if (hitInterface)
	{
		hitInterface->AddSoul(this);
		SpawnPickupSystem();
		SpawnPickupSound();
		Destroy();
	}
}