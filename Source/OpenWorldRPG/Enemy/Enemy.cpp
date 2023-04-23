// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Components/AttributeComponent.h"
#include "../HUD/HealthBarComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	healthBarComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	healthBarComponent->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (healthBarComponent)
	{
		healthBarComponent->SetVisibility(false);
	}

}
void AEnemy::Die()
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && dieMontage)
	{
		animInstance->Montage_Play(dieMontage);

		const int32 selection = FMath::RandRange(0, 2);
		FName sectionName = FName();
		switch (selection)
		{
		case 0:
			sectionName = FName("Death1");
			deathPose = EDeathPose::EDP_Death1;
			break;
		case 1:
			sectionName = FName("Death2");
			deathPose = EDeathPose::EDP_Death2;
			break;
		case 2:
			sectionName = FName("Death3");
			deathPose = EDeathPose::EDP_Death3;
			break;
		default:
			break;
		}
		animInstance->Montage_JumpToSection(sectionName, dieMontage);
	}
	if (healthBarComponent)
	{
		healthBarComponent->SetVisibility(false);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(3.f);
}
void AEnemy::PlayHitMontage(const FName& _sectionName)
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && hitMontage)
	{
		animInstance->Montage_Play(hitMontage);
		animInstance->Montage_JumpToSection(_sectionName, hitMontage);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (combatTarget)
	{
		const double distanceToTarget = (combatTarget->GetActorLocation() - GetActorLocation()).Size();
		if (distanceToTarget > combatRadius)
		{
			combatTarget = nullptr;
			if (healthBarComponent)
			{
				healthBarComponent->SetVisibility(false);
			}
		}
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& _point)
{
	if (healthBarComponent)
	{
		healthBarComponent->SetVisibility(true);
	}
	if (attributes && attributes->IsAlive())
	{
		DirectionalHitReact(_point);
	}
	else
	{
		Die();
	}
	

	if (hitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, hitSound, _point);
	}
	if (hitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), hitParticles, _point);
	}
}

void AEnemy::DirectionalHitReact(const FVector& _point)
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

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (attributes && healthBarComponent)
	{
		attributes->ReceiveDamage(DamageAmount);
		healthBarComponent->SetHealth(attributes->GetHealth());
	}
	combatTarget = EventInstigator->GetPawn();
	return DamageAmount;
}