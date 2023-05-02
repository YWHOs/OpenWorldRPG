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
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
void ABaseCharacter::Attack()
{
}
void ABaseCharacter::Die()
{
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
	return PlayRandomMontageSection(deathMontage, deathMontageSections);
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