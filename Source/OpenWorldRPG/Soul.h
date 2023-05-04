// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Soul.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API ASoul : public AItem
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere)
	int32 soul;

	double desiredZ;

	UPROPERTY(EditAnywhere)
	float driftRate = -15.f;

public:
	FORCEINLINE int32 GetSoul() const { return soul; }
	FORCEINLINE void SetSoul(int32 _soul) { soul = _soul; }
};
