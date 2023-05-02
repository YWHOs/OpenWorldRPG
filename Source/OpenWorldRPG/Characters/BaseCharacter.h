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


	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Attack();
	virtual void Die();

	virtual void PlayAttackMontage();
	void PlayHitMontage(const FName& _sectionName);
	void DirectionalHitReact(const FVector& _point);

	virtual bool CanAttack();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* equipWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* attackMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* hitMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* dieMontage;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* attributes;

	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* hitSound;
	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* hitParticles;
};
