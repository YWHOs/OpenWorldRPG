// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarComponent.h"
#include "HealthBar.h"
#include "Components/ProgressBar.h"


void UHealthBarComponent::SetHealth(float _percent)
{
	if (healthBarWidget == nullptr)
	{
		healthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}
	
	if (healthBarWidget && healthBarWidget->healthBar)
	{
		healthBarWidget->healthBar->SetPercent(_percent);
	}
}