// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GroomComponent.h"
#include "../Item.h"
#include "../Weapon.h"
#include "Animation/AnimMontage.h"

// Sets default values
ARPGCharacter::ARPGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

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

void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Tags.Add(FName("EngageTarget"));
}

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
float ARPGCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	return DamageAmount;
}
void ARPGCharacter::GetHit_Implementation(const FVector& _point, AActor* _hitter)
{
	Super::GetHit_Implementation(_point, _hitter);
	SetWeaponCollision(ECollisionEnabled::NoCollision);
	actionState = EActionState::EAS_HitReaction;
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
		EquipWeapon(overlapWeapon);
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
	Super::Attack();
	if (CanAttack())
	{
		PlayAttackMontage();
		actionState = EActionState::EAS_Attacking;
	}
}
void ARPGCharacter::EquipWeapon(AWeapon* _weapon)
{
	_weapon->Equip(GetMesh(), FName("right_hand_socket"), this, this);
	characterState = ECharacterState::ECS_EquippedOneHand;
	overlapItem = nullptr;
	equipWeapon = _weapon;
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

void ARPGCharacter::HitReactEnd()
{
	actionState = EActionState::EAS_Unoccupied;
}