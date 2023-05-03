// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENWORLDRPG_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenStamina(float _deltaTime);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float health;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float maxHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float stamina;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float maxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 gold;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 soul;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float dodgeCost = 14.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float staminaRegen = 8.f;
public:
	void ReceiveDamage(float _damage);
	void UseStamina(float _stamina);
	float GetHealth();
	float GetStaminaPercent();
	bool IsAlive();

	void AddGold(int32 _gold);
	void AddSoul(int32 _soul);
	FORCEINLINE int32 GetGold() const { return gold; }
	FORCEINLINE int32 GetSoul() const { return soul; }
	FORCEINLINE float GetDodgeCost() const { return dodgeCost; }
	FORCEINLINE float GetStamina() const { return stamina; }
};
