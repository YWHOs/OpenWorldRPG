// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Characters/BaseCharacter.h"
#include "../Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UPawnSensingComponent;
UCLASS()
class OPENWORLDRPG_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	// AActor
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;

	// IHitInterface
	virtual void GetHit_Implementation(const FVector& _point, AActor* _hitter) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ABaseCharacter
	virtual void Die_Implementation() override;
	void SpawnSoul();
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float _damage) override;
	virtual void AttackEnd() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EEnemyState enemyState = EEnemyState::EES_Patrolling;

private:
	void InitializeEnemy();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void PatrolTimerFinish();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsChasing();
	bool IsDead();
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();
	bool IsTargetRange(AActor* _target, double _radius);
	void MoveToTarget(AActor* _target);
	AActor* ChoosePatrolTarget();
	void SpawnDefaultWeapon();

	// Callback OnPawnSeen in UPawnSensing
	UFUNCTION()
	void PawnSeen(APawn* _seePawn);

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* healthBarComponent;
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* pawnSense;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> weaponClass;

	UPROPERTY(EditAnywhere)
	double combatRadius = 1000.f;
	UPROPERTY(EditAnywhere)
	double attackRadius = 150.f;
	UPROPERTY(EditAnywhere)
	double acceptanceRadius = 50.f;

	// NAV
	UPROPERTY()
	class AAIController* enemyController;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* patrolTarget;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> patrolTargets;

	UPROPERTY(EditAnywhere)
	double patrolRadius = 200.f;

	FTimerHandle patrolTimer;
	// Patrol Wait Time
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float waitMin = 3.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float waitMax = 5.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float patrollingSpeed = 125.f;

	FTimerHandle attackTimer;
	UPROPERTY(EditAnywhere, Category = Combat)
	float attackMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float attackMax = 1.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float chasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float deathLifeSpan = 8.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class ASoul> soulClass;
};
