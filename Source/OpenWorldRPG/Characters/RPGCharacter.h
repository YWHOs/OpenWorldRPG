// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "RPGCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;
class AWeapon;

UCLASS()
class OPENWORLDRPG_API ARPGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARPGCharacter();
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(float _value);
	void MoveRight(float _value);
	void Turn(float _value);
	void LookUp(float _value);
	void EKeyPressed();
	void Attack();

	// Montage
	void PlayAttackMontage();
	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	bool CanAttack();
	void PlayEquipMontage(FName _sectionName);
	bool CanDisarm();
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void FinishEquip();

private:
	ECharacterState characterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState actionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* spring;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* camera;

	UPROPERTY(VisibleAnywhere)
	UGroomComponent* hair;
	UPROPERTY(VisibleAnywhere)
	UGroomComponent* eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* overlapItem;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* equipWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* attackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* equipMontage;
public:
	FORCEINLINE void SetOverlapItem(AItem* _item) { overlapItem = _item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return characterState; }
};
