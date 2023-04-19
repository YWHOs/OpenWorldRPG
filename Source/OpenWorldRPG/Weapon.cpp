// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Characters\/RPGCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

AWeapon::AWeapon()
{
	weaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	weaponBox->SetupAttachment(GetRootComponent());
}
void AWeapon::Equip(USceneComponent* _parent, FName _socketName)
{
	AttachMeshToSocket(_parent, _socketName);
	itemState = EItemState::EIS_Equipped;
	if (equipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, equipSound, GetActorLocation());
	}
	if (sphere)
	{
		sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
void AWeapon::AttachMeshToSocket(USceneComponent* _parent, const FName& _socketName)
{
	FAttachmentTransformRules transformRules(EAttachmentRule::SnapToTarget, true);
	itemMesh->AttachToComponent(_parent, transformRules, _socketName);
}
void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
