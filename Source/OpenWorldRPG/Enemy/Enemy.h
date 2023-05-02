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
	// Sets default values for this character's properties
	AEnemy();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void CheckPatrolTarget();
	void CheckCombatTarget();
	virtual void GetHit_Implementation(const FVector& _point) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Die() override;
	bool IsTargetRange(AActor* _target, double _radius);
	void MoveToTarget(AActor* _target);
	AActor* ChoosePatrolTarget();
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float _damage) override;
	virtual int32 PlayDeathMontage() override;

	UPROPERTY(EditAnywhere, Category = Combat)
	float deathLifeSpan = 8.f;

	UFUNCTION()
	void PawnSeen(APawn* _seePawn);

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> deathPose;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState enemyState = EEnemyState::EES_Patrolling;

private:

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* healthBarComponent;
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* pawnSense;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> weaponClass;
	UPROPERTY()
	AActor* combatTarget;

	UPROPERTY(EditAnywhere)
	double combatRadius = 500.f;
	UPROPERTY(EditAnywhere)
	double attackRadius = 150.f;

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
	void PatrolTimerFinish();

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float waitMin = 3.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float waitMax = 5.f;

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
	FTimerHandle attackTimer;
	UPROPERTY(EditAnywhere, Category = Combat)
	float attackMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float attackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float patrollingSpeed = 125.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float chasingSpeed = 300.f;

public:	


};
