// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UAttributeComponent::ReceiveDamage(float _damage)
{
	health = FMath::Clamp(health - _damage, 0.f, maxHealth);
}
void UAttributeComponent::UseStamina(float _stamina)
{
	stamina = FMath::Clamp(stamina - _stamina, 0.f, maxStamina);
}
float UAttributeComponent::GetHealth()
{
	return health / maxHealth;
}
float UAttributeComponent::GetStaminaPercent()
{
	return stamina / maxStamina;
}
bool UAttributeComponent::IsAlive()
{
	return health > 0.f;
}
void UAttributeComponent::AddGold(int32 _gold)
{
	gold += _gold;
}
void UAttributeComponent::AddSoul(int32 _soul)
{
	soul += _soul;
}
// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAttributeComponent::RegenStamina(float _deltaTime)
{
	stamina = FMath::Clamp(stamina + staminaRegen * _deltaTime, 0.f, maxStamina);
}