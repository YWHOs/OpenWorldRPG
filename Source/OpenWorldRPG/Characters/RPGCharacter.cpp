// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "../Item.h"
#include "../Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"

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
	
	Tags.Add(FName("Player"));
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

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Equip"), IE_Pressed, this, &ARPGCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &ARPGCharacter::Attack);
}
void ARPGCharacter::SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (equipWeapon && equipWeapon->GetWeaponBox())
	{
		equipWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		equipWeapon->ignoreActors.Empty();
	}
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
	if (actionState != EActionState::EAS_Unoccupied) return;
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
void ARPGCharacter::EKeyPressed()
{
	AWeapon* overlapWeapon = Cast<AWeapon>(overlapItem);
	if (overlapWeapon)
	{
		overlapWeapon->Equip(GetMesh(), FName("right_hand_socket"), this, this);
		characterState = ECharacterState::ECS_EquippedOneHand;
		overlapItem = nullptr;
		equipWeapon = overlapWeapon;
	}
	else
	{
		if (CanDisarm())
		{
			PlayEquipMontage(FName("Unequip"));
			characterState = ECharacterState::ECS_Unequipped;
			actionState = EActionState::EAS_EquipWeapon;
		}
		else if (CanArm())
		{
			PlayEquipMontage(FName("Equip"));
			characterState = ECharacterState::ECS_EquippedOneHand;
			actionState = EActionState::EAS_EquipWeapon;
		}
	}
}
void ARPGCharacter::Attack()
{
	if (CanAttack())
	{
		PlayAttackMontage();
		actionState = EActionState::EAS_Attacking;
	}
}
bool ARPGCharacter::CanAttack()
{
	return actionState == EActionState::EAS_Unoccupied && characterState != ECharacterState::ECS_Unequipped;
}
bool ARPGCharacter::CanDisarm()
{
	return actionState == EActionState::EAS_Unoccupied && characterState != ECharacterState::ECS_Unequipped;
}
bool ARPGCharacter::CanArm()
{
	return actionState == EActionState::EAS_Unoccupied && characterState == ECharacterState::ECS_Unequipped && equipWeapon;
}
void ARPGCharacter::Disarm()
{
	if (equipWeapon)
	{
		equipWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}
void ARPGCharacter::Arm()
{
	if (equipWeapon)
	{
		equipWeapon->AttachMeshToSocket(GetMesh(), FName("right_hand_socket"));
	}
}
void ARPGCharacter::FinishEquip()
{
	actionState = EActionState::EAS_Unoccupied;
}
void ARPGCharacter::PlayAttackMontage()
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && attackMontage)
	{
		animInstance->Montage_Play(attackMontage);
		const int32 selection = FMath::RandRange(0, 1);
		FName sectionName = FName();
		switch (selection)
		{
		case 0:
			sectionName = FName("Attack1");
			break;
		case 1:
			sectionName = FName("Attack2");
			break;
		default:
			break;
		}
		animInstance->Montage_JumpToSection(sectionName, attackMontage);
	}
}
void ARPGCharacter::PlayEquipMontage(const FName& _sectionName)
{
	UAnimInstance* animInstace = GetMesh()->GetAnimInstance();
	if (animInstace && equipMontage)
	{
		animInstace->Montage_Play(equipMontage);
		animInstace->Montage_JumpToSection(_sectionName, equipMontage);
	}
}
void ARPGCharacter::AttackEnd()
{
	actionState = EActionState::EAS_Unoccupied;
}