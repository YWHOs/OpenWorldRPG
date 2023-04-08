// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Bird.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class OPENWORLDRPG_API ABird : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABird();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(float _value);
	void Turn(float _value);
	void LookUp(float _value);

private:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* capsule;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* birdMesh;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* spring;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* camera;
};
