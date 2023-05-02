// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Components/AttributeComponent.h"
#include "../HUD/HealthBarComponent.h"
#include "AIController.h"
#include "../Weapon.h"
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
	HideHealthBar();

	enemyController = Cast<AAIController>(GetController());
	MoveToTarget(patrolTarget);

	if (pawnSense)
	{
		pawnSense->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	UWorld* world = GetWorld();
	if (world && weaponClass)
	{
		AWeapon* defaultWeapon = world->SpawnActor<AWeapon>(weaponClass);
		defaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		equipWeapon = defaultWeapon;
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
	HideHealthBar();
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
	moveRequest.SetAcceptanceRadius(50.f);
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
void AEnemy::Attack()
{
	Super::Attack();
	PlayAttackMontage();
}
void AEnemy::PlayAttackMontage()
{
	Super::PlayAttackMontage();

	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && attackMontage)
	{
		animInstance->Montage_Play(attackMontage);
		const int32 selection = FMath::RandRange(0, 1);
		FName sectionName = FName();
		switch (selection)
		{
		case 0:
			sectionName = FName("Attack1");
			break;
		case 1:
			sectionName = FName("Attack2");
			break;
		default:
			break;
		}
		animInstance->Montage_JumpToSection(sectionName, attackMontage);
	}
}
void AEnemy::PawnSeen(APawn* _seePawn)
{
	const bool shouldChase = enemyState != EEnemyState::EES_Dead && enemyState != EEnemyState::EES_Chasing && enemyState < EEnemyState::EES_Attacking && _seePawn->ActorHasTag(FName("Player"));

	if (shouldChase)
	{
		combatTarget = _seePawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsDead()) return;
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
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (enemyState != EEnemyState::EES_Engaged)
		{
			StartPatrolling();
		}
	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();
		if (enemyState != EEnemyState::EES_Engaged)
		{
			ChaseTarget();
		}
	}
	else if (CanAttack())
	{
		StartAttackTimer();
	}
}
bool AEnemy::CanAttack()
{
	bool canAttack = !IsOutsideAttackRadius() && enemyState != EEnemyState::EES_Attacking && !IsDead();
	return canAttack;
}
void AEnemy::HandleDamage(float _damage)
{
	Super::HandleDamage(_damage);
	if (attributes && healthBarComponent)
	{
		healthBarComponent->SetHealth(attributes->GetHealth());
	}
}

void AEnemy::GetHit_Implementation(const FVector& _point)
{
	ShowHealthBar();
	if (IsAlive())
	{
		DirectionalHitReact(_point);
	}
	else
	{
		Die();
	}
	PlayHitSound(_point);
	SpawnHitParticle(_point);
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	combatTarget = EventInstigator->GetPawn();
	ChaseTarget();
	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (equipWeapon)
	{
		equipWeapon->Destroy();
	}
}

void AEnemy::HideHealthBar()
{
	if (healthBarComponent)
	{
		healthBarComponent->SetVisibility(false);
	}
}
void AEnemy::ShowHealthBar()
{
	if (healthBarComponent)
	{
		healthBarComponent->SetVisibility(true);
	}
}
void AEnemy::LoseInterest()
{
	combatTarget = nullptr;
	HideHealthBar();
}
void AEnemy::StartPatrolling()
{
	enemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = patrollingSpeed;
	MoveToTarget(patrolTarget);
}
void AEnemy::ChaseTarget()
{
	enemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = chasingSpeed;
	MoveToTarget(combatTarget);
}
bool AEnemy::IsOutsideCombatRadius()
{
	return !IsTargetRange(combatTarget, combatRadius);
}
bool AEnemy::IsOutsideAttackRadius()
{
	return !IsTargetRange(combatTarget, attackRadius);
}
bool AEnemy::IsChasing()
{
	return enemyState == EEnemyState::EES_Chasing;
}
bool AEnemy::IsDead()
{
	return enemyState == EEnemyState::EES_Dead;
}
void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(patrolTimer);
}
void AEnemy::StartAttackTimer()
{
	enemyState = EEnemyState::EES_Attacking;
	const float attackTime = FMath::RandRange(attackMin, attackMax);
	GetWorldTimerManager().SetTimer(attackTimer, this, &AEnemy::Attack, attackTime);
}
void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(attackTimer);
}