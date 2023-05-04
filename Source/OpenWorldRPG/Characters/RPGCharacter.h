// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "../PickupInterface.h"
#include "RPGCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class ASoul;
class ATreasure;
class UAnimMontage;
class UUOverlay;

UCLASS()
class OPENWORLDRPG_API ARPGCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARPGCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	// IHitInterface
	virtual void GetHit_Implementation(const FVector& _point, AActor* _hitter) override;
	virtual void SetOverlapItem(AItem* _item) override;
	virtual void AddSoul(ASoul* _soul) override;
	virtual void AddGold(ATreasure* _gold) override;

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
	void Dodge();

	void EquipWeapon(AWeapon* _weapon);
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	virtual bool CanAttack() override;
	void PlayEquipMontage(const FName& _sectionName);
	bool CanDisarm();
	bool CanArm();
	virtual void Die_Implementation() override;
	bool HasEnoughStamina();

	UFUNCTION(BlueprintCallable)
	void Disarm();
	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void FinishEquip();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

private:
	void InitializeOverlay();
	void SetHUDHealth();

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

	UPROPERTY()
	UUOverlay* overlay;
public:
	FORCEINLINE ECharacterState GetCharacterState() const { return characterState; }
	FORCEINLINE EActionState GetActionState() const { return actionState; }
};
