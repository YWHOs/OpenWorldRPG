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
UCLASS()
class OPENWORLDRPG_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& _point) override;

	void DirectionalHitReact(const FVector& _point);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Die();
	void PlayHitMontage(const FName& _sectionName);

	UPROPERTY(BlueprintReadOnly)
	EDeathPose deathPose = EDeathPose::EDP_Alive;

private:
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* attributes;
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* healthBarComponent;

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

public:	


};
