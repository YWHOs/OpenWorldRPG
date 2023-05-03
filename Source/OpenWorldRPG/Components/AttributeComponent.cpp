// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
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
float UAttributeComponent::GetHealth()
{
	return health / maxHealth;
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

	// ...
}

