// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "RPGAnimInstance.generated.h"


/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API URPGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float _time) override;

	UPROPERTY(BlueprintReadOnly)
	class ARPGCharacter* character;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* characterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float groundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool isFalling;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	ECharacterState characterState;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	EActionState actionState;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	TEnumAsByte<EDeathPose> deathPose;
};
