// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UOverlay.generated.h"

/**
 * 
 */
class UProgressBar;
class UTextBlock;
UCLASS()
class OPENWORLDRPG_API UUOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetHealthBar(float _percent);
	void SetStaminaBar(float _percent);
	void SetGold(int32 _gold);
	void SetSoul(int32 _soul);
private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SoulText;
};
