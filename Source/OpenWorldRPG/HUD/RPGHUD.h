// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RPGHUD.generated.h"

class UUOverlay;
/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API ARPGHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUOverlay> overlayClass;

	UPROPERTY()
	UUOverlay* overlay;
};
