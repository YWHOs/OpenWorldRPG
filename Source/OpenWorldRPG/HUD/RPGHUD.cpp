// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGHUD.h"
#include "UOverlay.h"

void ARPGHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* controller = world->GetFirstPlayerController();
		if (controller && overlayClass)
		{
			overlay = CreateWidget<UUOverlay>(controller, overlayClass);
			overlay->AddToViewport();
		}
	}

}