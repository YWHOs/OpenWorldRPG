// Fill out your copyright notice in the Description page of Project Settings.


#include "UOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UUOverlay::SetHealthBar(float _percent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(_percent);
	}
}

void UUOverlay::SetStaminaBar(float _percent)
{
	if (StaminaBar)
	{
		StaminaBar->SetPercent(_percent);
	}
}

void UUOverlay::SetGold(int32 _gold)
{
	if (GoldText)
	{
		GoldText->SetText(FText::FromString(FString::Printf(TEXT("d"), _gold)));
	}
}
void UUOverlay::SetSoul(int32 _soul)
{
	if (SoulText)
	{
		SoulText->SetText(FText::FromString(FString::Printf(TEXT("d"), _soul)));
	}
}