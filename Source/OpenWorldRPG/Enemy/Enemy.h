// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../HitInterface.h"
#include "../Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UAttributeComponent;
class UHealthBarComponent;
class UPawnSensingComponent;
UCLASS()
class OPENWORLDRPG_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void CheckPatrolTarget();
	void CheckCombatTarget();
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& _point) override;

	void DirectionalHitReact(const FVector& _point);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Die();
	bool IsTargetRange(AActor* _target, double _radius);
	void MoveToTarget(AActor* _target);
	AActor* ChoosePatrolTarget();
	UFUNCTION()
	void PawnSeen(APawn* _seePawn);
	void PlayHitMontage(const FName& _sectionName);

	UPROPERTY(BlueprintReadOnly)
	EDeathPose deathPose = EDeathPose::EDP_Alive;

private:

	//Components
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* attributes;
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* healthBarComponent;
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* pawnSense;

	//Animation
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* dieMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* hitMontage;

	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* hitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* hitParticles;

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

	EEnemyState enemyState = EEnemyState::EES_Patrolling;
public:	


};
