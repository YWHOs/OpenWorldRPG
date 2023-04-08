// Fill out your copyright notice in the Description page of Project Settings.


#include "Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ABird::ABird()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	capsule->SetCapsuleHalfHeight(20.f);
	capsule->SetCapsuleRadius(15.f);
	SetRootComponent(capsule);

	birdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	birdMesh->SetupAttachment(capsule);

	spring = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	spring->SetupAttachment(capsule);
	spring->TargetArmLength = 300.f;

	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	camera->SetupAttachment(spring);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ABird::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABird::MoveForward(float _value)
{
	if (Controller && _value != 0.f)
	{
		FVector forward = GetActorForwardVector();
		AddMovementInput(forward, _value);
	}
}
void ABird::Turn(float _value)
{
	AddControllerYawInput(_value);
}
void ABird::LookUp(float _value)
{
	AddControllerPitchInput(_value);
}

// Called every frame
void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABird::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ABird::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ABird::LookUp);
}

