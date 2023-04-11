// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"

// Sets default values
ARPGCharacter::ARPGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	spring = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	spring->SetupAttachment(GetRootComponent());
	spring->TargetArmLength = 300.f;

	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	camera->SetupAttachment(spring);

	hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	hair->SetupAttachment(GetMesh());
	hair->AttachmentName = FString("head");

	eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	eyebrows->SetupAttachment(GetMesh());
	eyebrows->AttachmentName = FString("head");
}

// Called when the game starts or when spawned
void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ARPGCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ARPGCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ARPGCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ARPGCharacter::LookUp);
}

void ARPGCharacter::MoveForward(float _value)
{
	if (Controller && _value != 0.f)
	{
		const FRotator controlRotation = GetControlRotation();
		const FRotator yawRotation(0.f, controlRotation.Yaw, 0.f);

		const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(direction, _value);
		//FVector forward = GetActorForwardVector();
		//AddMovementInput(forward, _value);
	}
}
void ARPGCharacter::MoveRight(float _value)
{
	if (Controller && _value != 0.f)
	{
		const FRotator controlRotation = GetControlRotation();
		const FRotator yawRotation(0.f, controlRotation.Yaw, 0.f);

		const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(direction, _value);
		//FVector right = GetActorRightVector();
		//AddMovementInput(right, _value);
	}
}
void ARPGCharacter::Turn(float _value)
{
	AddControllerYawInput(_value);
}
void ARPGCharacter::LookUp(float _value)
{
	AddControllerPitchInput(_value);
}