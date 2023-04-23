// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Components/AttributeComponent.h"
#include "Components/WidgetComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	healthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	healthBarWidget->SetupAttachment(GetRootComponent());
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
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

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& _point)
{
	DirectionalHitReact(_point);

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
