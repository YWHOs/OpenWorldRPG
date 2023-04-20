// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../HitInterface.h"
#include "Enemy.generated.h"

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

	virtaul void GetHit(const FVector& _point) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	


};
