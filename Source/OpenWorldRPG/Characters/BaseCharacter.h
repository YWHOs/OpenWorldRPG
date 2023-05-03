// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class OPENWORLDRPG_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetHit_Implementation(const FVector& _point, AActor* _hitter) override;
	virtual void Attack();
	virtual void Die();
	void DirectionalHitReact(const FVector& _point);
	virtual void HandleDamage(float _damage);
	void PlayHitSound(const FVector& _point);
	void SpawnHitParticle(const FVector& _point);
	void DisableCapsule();
	virtual bool CanAttack();
	bool IsAlive();

	void PlayHitMontage(const FName& _sectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* equipWeapon;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* attributes;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	AActor* combatTarget;

	UPROPERTY(EditAnywhere, Category = Combat)
	double warpTargetDistance = 75.f;

private:
	void PlayMontageSection(UAnimMontage* _montage, const FName& _sectionName);
	int32 PlayRandomMontageSection(UAnimMontage* _montage, const TArray<FName>& _sectionNames);

	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* hitSound;
	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* hitParticles;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* attackMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* hitMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* deathMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> attackMontageSections;
	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> deathMontageSections;

};
