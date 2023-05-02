// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Components/AttributeComponent.h"
#include "../HUD/HealthBarComponent.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	healthBarComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	healthBarComponent->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	pawnSense = CreateDefaultSubobject< UPawnSensingComponent>(TEXT("PawnSensing"));
	pawnSense->SightRadius = 4000.f;
	pawnSense->SetPeripheralVisionAngle(45.f);
}
void AEnemy::PatrolTimerFinish()
{
	MoveToTarget(patrolTarget);
}
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (healthBarComponent)
	{
		healthBarComponent->SetVisibility(false);
	}

	enemyController = Cast<AAIController>(GetController());
	MoveToTarget(patrolTarget);

	if (pawnSense)
	{
		pawnSense->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
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
bool AEnemy::IsTargetRange(AActor* _target, double _radius)
{
	if (_target == nullptr) return false;
	const double distanceToTarget = (_target->GetActorLocation() - GetActorLocation()).Size();
	return distanceToTarget <= _radius;
}
void AEnemy::MoveToTarget(AActor* _target)
{
	if (enemyController == nullptr || _target == nullptr) return;
	FAIMoveRequest moveRequest;
	moveRequest.SetGoalActor(_target);
	moveRequest.SetAcceptanceRadius(15.f);
	enemyController->MoveTo(moveRequest);
}
AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> validTargets;
	for (AActor* target : patrolTargets)
	{
		if (target != patrolTarget)
		{
			validTargets.AddUnique(target);
		}
	}
	const int32 patrolTargetNum = validTargets.Num();
	if (patrolTargetNum > 0)
	{
		const int32 selection = FMath::RandRange(0, patrolTargetNum - 1);
		return validTargets[selection];
	}
	return nullptr;
}
void AEnemy::PawnSeen(APawn* _seePawn)
{
	if (enemyState == EEnemyState::EES_Chasing) return;
	if (_seePawn->ActorHasTag(FName("Player")))
	{
		GetWorldTimerManager().ClearTimer(patrolTimer);
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		combatTarget = _seePawn;

		if (enemyState != EEnemyState::EES_Attacking)
		{
			enemyState = EEnemyState::EES_Chasing;
			MoveToTarget(combatTarget);
		}
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (enemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (IsTargetRange(patrolTarget, patrolRadius))
	{
		patrolTarget = ChoosePatrolTarget();
		const float waitTime = FMath::RandRange(waitMin, waitMax);
		GetWorldTimerManager().SetTimer(patrolTimer, this, &AEnemy::PatrolTimerFinish, waitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (!IsTargetRange(combatTarget, combatRadius))
	{
		// Outside combat radius
		combatTarget = nullptr;
		if (healthBarComponent)
		{
			healthBarComponent->SetVisibility(false);
		}
		enemyState = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = 125.f;
		MoveToTarget(patrolTarget);
	}
	else if (!IsTargetRange(combatTarget, attackRadius) && enemyState != EEnemyState::EES_Chasing)
	{
		enemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		MoveToTarget(combatTarget);
	}
	else if (IsTargetRange(combatTarget, attackRadius) && enemyState != EEnemyState::EES_Attacking)
	{
		enemyState = EEnemyState::EES_Attacking;
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

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (attributes && healthBarComponent)
	{
		attributes->ReceiveDamage(DamageAmount);
		healthBarComponent->SetHealth(attributes->GetHealth());
	}
	combatTarget = EventInstigator->GetPawn();
	enemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	MoveToTarget(combatTarget);
	return DamageAmount;
}