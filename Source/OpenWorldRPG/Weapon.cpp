// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Characters\/RPGCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	weaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	weaponBox->SetupAttachment(GetRootComponent());
	weaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	weaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	weaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	boxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	boxTraceStart->SetupAttachment(GetRootComponent());
	boxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	boxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	weaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}
void AWeapon::Equip(USceneComponent* _parent, FName _socketName, AActor* _owner, APawn* _instigator)
{
	SetOwner(_owner);
	SetInstigator(_instigator);
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
	if (emberEffect)
	{
		emberEffect->Deactivate();
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

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector start = boxTraceStart->GetComponentLocation();
	const FVector end = boxTraceEnd->GetComponentLocation();

	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);

	for (AActor* actor : ignoreActors)
	{
		actorsToIgnore.AddUnique(actor);
	}
	FHitResult hit;
	UKismetSystemLibrary::BoxTraceSingle(this, start, end, FVector(5.f, 5.f, 5.f), boxTraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, false, actorsToIgnore, EDrawDebugTrace::None, hit, true);
	if (hit.GetActor())
	{
		// Damage
		UGameplayStatics::ApplyDamage(hit.GetActor(), damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());

		IHitInterface* hitInterface = Cast<IHitInterface>(hit.GetActor());
		if (hitInterface)
		{
			hitInterface->Execute_GetHit(hit.GetActor(), hit.ImpactPoint);
		}
		ignoreActors.AddUnique(hit.GetActor());

		CreateFields(hit.ImpactPoint);

	}
}
