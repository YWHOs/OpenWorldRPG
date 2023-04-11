// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGAnimInstance.h"
#include "RPGCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void URPGAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	character = Cast<ARPGCharacter>(TryGetPawnOwner());
	if (character)
	{
		characterMovement = character->GetCharacterMovement();
	}

}

void URPGAnimInstance::NativeUpdateAnimation(float _time)
{
	Super::NativeUpdateAnimation(_time);

	if (characterMovement)
	{
		groundSpeed = UKismetMathLibrary::VSizeXY(characterMovement->Velocity);
	}
}