// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "../Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "../Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
void ABaseCharacter::GetHit_Implementation(const FVector& _point, AActor* _hitter)
{
	if (IsAlive() && _hitter)
	{
		DirectionalHitReact(_hitter->GetActorLocation());
	}
	else
	{
		Die();
	}
	PlayHitSound(_point);
	SpawnHitParticle(_point);
}
void ABaseCharacter::Attack()
{
	if (combatTarget && combatTarget->ActorHasTag(FName("Dead")))
	{
		combatTarget = nullptr;
	}
}
void ABaseCharacter::Die()
{
	Tags.Add(FName("Dead"));
	PlayDeathMontage();
}
void ABaseCharacter::PlayHitMontage(const FName& _sectionName)
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && hitMontage)
	{
		animInstance->Montage_Play(hitMontage);
		animInstance->Montage_JumpToSection(_sectionName, hitMontage);
	}
}
void ABaseCharacter::DirectionalHitReact(const FVector& _point)
{
	const FVector forward = GetActorForwardVector();
	const FVector impactLower(_point.X, _point.Y, GetActorLocation().Z);
	const FVector hit = (impactLower - GetActorLocation()).GetSafeNormal();

	// Forward * Hit = |Forward||Hit| * cos(theta)
	const double cosTheta = FVector::DotProduct(forward, hit);
	// Take inverse cosine of cos to get theta
	double theta = FMath::Acos(cosTheta);
	theta = FMath::RadiansToDegrees(theta);

	const FVector crossProduct = FVector::CrossProduct(forward, hit);
	if (crossProduct.Z < 0)
	{
		theta *= -1.f;
	}
	FName section("FromBack");
	if (theta >= -45.f && theta < 45.f)
	{
		section = FName("FromFront");
	}
	else if (theta >= -135.f && theta < -45.f)
	{
		section = FName("FromLeft");
	}
	else if (theta >= 45.f && theta < 135.f)
	{
		section = FName("FromRight");
	}
	PlayHitMontage(FName(section));
}
void ABaseCharacter::PlayHitSound(const FVector& _point)
{
	if (hitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, hitSound, _point);
	}
}
void ABaseCharacter::SpawnHitParticle(const FVector& _point)
{
	if (hitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), hitParticles, _point);
	}
}
void ABaseCharacter::HandleDamage(float _damage)
{
	if (attributes)
	{
		attributes->ReceiveDamage(_damage);
	}
}
void ABaseCharacter::PlayMontageSection(UAnimMontage* _montage, const FName& _sectionName)
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && _montage)
	{
		animInstance->Montage_Play(_montage);
		animInstance->Montage_JumpToSection(_sectionName, _montage);
	}
}
int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* _montage, const TArray<FName>& _sectionNames)
{
	if (_sectionNames.Num() <= 0) return -1;
	const int32 max = _sectionNames.Num() - 1;
	const int32 selection = FMath::RandRange(0, max);
	PlayMontageSection(_montage, _sectionNames[selection]);
	return selection;
}
int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(attackMontage, attackMontageSections);
}
int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 selection = PlayRandomMontageSection(deathMontage, deathMontageSections);
	TEnumAsByte<EDeathPose> pose(selection);
	if (pose < EDeathPose::EDP_MAX)
	{
		deathPose = pose;
	}
	return selection;
}
void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance)
	{
		animInstance->Montage_Stop(0.25f, attackMontage);
	}
}
FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (combatTarget == nullptr) return FVector();

	const FVector targetLocation = combatTarget->GetActorLocation();
	const FVector location = GetActorLocation();

	FVector targetToMe = (location - targetLocation).GetSafeNormal();
	targetToMe *= warpTargetDistance;

	return targetLocation + targetToMe;
}
FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (combatTarget)
	{
		return combatTarget->GetActorLocation();
	}
	return FVector();
}
void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
bool ABaseCharacter::CanAttack()
{
	return false;
}
bool ABaseCharacter::IsAlive()
{
	return attributes && attributes->IsAlive();
}
void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
void ABaseCharacter::AttackEnd()
{
}
// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (equipWeapon && equipWeapon->GetWeaponBox())
	{
		equipWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		equipWeapon->ignoreActors.Empty();
	}
}