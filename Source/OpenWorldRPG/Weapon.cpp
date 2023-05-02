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
	itemState = EItemState::EIS_Equipped;
	SetOwner(_owner);
	SetInstigator(_instigator);
	AttachMeshToSocket(_parent, _socketName);

	if (sphere)
	{
		sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (equipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, equipSound, GetActorLocation());
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

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (SameTypeActor(OtherActor)) return;

	FHitResult hit;
	BoxTrace(hit);
	if (hit.GetActor())
	{
		if (SameTypeActor(hit.GetActor())) return;
		// Damage
		UGameplayStatics::ApplyDamage(hit.GetActor(), damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(hit);
		CreateFields(hit.ImpactPoint);

	}
}
bool AWeapon::SameTypeActor(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}
void AWeapon::ExecuteGetHit(FHitResult& _hit)
{
	IHitInterface* hitInterface = Cast<IHitInterface>(_hit.GetActor());
	if (hitInterface)
	{
		hitInterface->Execute_GetHit(_hit.GetActor(), _hit.ImpactPoint, GetOwner());
	}
}
void AWeapon::BoxTrace(FHitResult& _hit)
{
	const FVector start = boxTraceStart->GetComponentLocation();
	const FVector end = boxTraceEnd->GetComponentLocation();

	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);

	for (AActor* actor : ignoreActors)
	{
		actorsToIgnore.AddUnique(actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(this, start, end, boxTraceExtent, boxTraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, false, actorsToIgnore, showBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, _hit, true);
	ignoreActors.AddUnique(_hit.GetActor());
}
