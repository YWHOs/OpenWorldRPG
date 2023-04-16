// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Characters\/RPGCharacter.h"


void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ARPGCharacter* character = Cast<ARPGCharacter>(OtherActor);
	if (character)
	{
		FAttachmentTransformRules transformRules(EAttachmentRule::SnapToTarget, true);
		itemMesh->AttachToComponent(character->GetMesh(), transformRules, FName("right_hand_socket"));
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
