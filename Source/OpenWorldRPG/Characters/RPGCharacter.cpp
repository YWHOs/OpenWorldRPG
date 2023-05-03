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
#include "../HUD/RPGHUD.h"
#include "../HUD/UOverlay.h"
#include "../Components/AttributeComponent.h"
#include "../Soul.h"
#include "../Treasure.h"

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
void ARPGCharacter::Tick(float DeltaTime)
{
	if (attributes && overlay)
	{
		attributes->RegenStamina(DeltaTime);
		overlay->SetStaminaBar(attributes->GetStaminaPercent());
	}
}
void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Tags.Add(FName("EngageTarget"));

	InitializeOverlay();
}

void ARPGCharacter::InitializeOverlay()
{
	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (playerController)
	{
		ARPGHUD* HUD = Cast<ARPGHUD>(playerController->GetHUD());
		if (HUD)
		{
			overlay = HUD->GetOverlay();
			if (overlay && attributes)
			{
				overlay->SetHealthBar(attributes->GetHealth());
				overlay->SetStaminaBar(1.f);
				overlay->SetGold(0);
				overlay->SetSoul(0);
			}
		}
	}
}

void ARPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ARPGCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ARPGCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ARPGCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ARPGCharacter::LookUp);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ARPGCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Equip"), IE_Pressed, this, &ARPGCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &ARPGCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("Dodge"), IE_Pressed, this, &ARPGCharacter::Dodge);
}
void ARPGCharacter::Jump()
{
	if (actionState == EActionState::EAS_Unoccupied)
	{
		Super::Jump();
	}

}
float ARPGCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}
void ARPGCharacter::SetHUDHealth()
{
	if (overlay && attributes)
	{
		overlay->SetHealthBar(attributes->GetHealth());
	}
}
void ARPGCharacter::GetHit_Implementation(const FVector& _point, AActor* _hitter)
{
	Super::GetHit_Implementation(_point, _hitter);
	SetWeaponCollision(ECollisionEnabled::NoCollision);
	if (attributes && attributes->GetHealth() > 0.f)
	{
		actionState = EActionState::EAS_HitReaction;
	}

}
void ARPGCharacter::SetOverlapItem(AItem* _item)
{
	overlapItem = _item;
}
void ARPGCharacter::AddSoul(ASoul* _soul)
{
	if (attributes && overlay)
	{
		attributes->AddSoul(_soul->GetSoul());
		overlay->SetSoul(attributes->GetSoul());
	}
}
void ARPGCharacter::AddGold(ATreasure* _gold)
{
	if (attributes && overlay)
	{
		attributes->AddGold(_gold->GetGold());
		overlay->SetGold(attributes->GetGold());
	}
}
void ARPGCharacter::MoveForward(float _value)
{
	if (actionState != EActionState::EAS_Unoccupied) return;
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
void ARPGCharacter::Dodge()
{
	if (actionState != EActionState::EAS_Unoccupied || !HasEnoughStamina()) return;

	PlayDodgeMontage();
	actionState = EActionState::EAS_Dodge;
	if (attributes && overlay)
	{
		attributes->UseStamina(attributes->GetDodgeCost());
		overlay->SetStaminaBar(attributes->GetStaminaPercent());
	}
}
bool ARPGCharacter::HasEnoughStamina()
{
	return attributes && attributes->GetStamina() > attributes->GetDodgeCost();
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
void ARPGCharacter::Die()
{
	Super::Die();

	actionState = EActionState::EAS_Dead;
	DisableMeshCollision();
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
void ARPGCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	actionState = EActionState::EAS_Unoccupied;
}

void ARPGCharacter::HitReactEnd()
{
	actionState = EActionState::EAS_Unoccupied;
}