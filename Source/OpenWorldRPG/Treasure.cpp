// Fill out your copyright notice in the Description page of Project Settings.


#include "Treasure.h"
#include "Characters/RPGCharacter.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARPGCharacter* character = Cast<ARPGCharacter>(OtherActor);
	if (character)
	{
		if (pickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, pickupSound, GetActorLocation());
		}
		Destroy();
	}
}