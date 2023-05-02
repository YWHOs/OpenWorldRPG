// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "RPGCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;

UCLASS()
class OPENWORLDRPG_API ARPGCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARPGCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// IHitInterface
	virtual void GetHit_Implementation(const FVector& _point, AActor* _hitter) override;

protected:
	// Callback input
	virtual void BeginPlay() override;

	// Input
	void MoveForward(float _value);
	void MoveRight(float _value);
	void Turn(float _value);
	void LookUp(float _value);
	void EKeyPressed();
	virtual void Attack() override;

	void EquipWeapon(AWeapon* _weapon);
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;
	void PlayEquipMontage(const FName& _sectionName);
	bool CanDisarm();
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Disarm();
	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void FinishEquip();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

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

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* equipMontage;
public:
	FORCEINLINE void SetOverlapItem(AItem* _item) { overlapItem = _item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return characterState; }
};
