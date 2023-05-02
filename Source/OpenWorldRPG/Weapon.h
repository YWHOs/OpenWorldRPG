// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;
/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	void Equip(USceneComponent* _parent, FName _socketName, AActor* _owner, APawn* _instigator);
	void AttachMeshToSocket(USceneComponent* _parent, const FName& _socketName);
	TArray<AActor*> ignoreActors;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool SameTypeActor(AActor* OtherActor);

	void ExecuteGetHit(FHitResult& _hit);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& _fieldLocation);

private:

	void BoxTrace(FHitResult& _hit);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector boxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool showBoxDebug = false;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* equipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* weaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* boxTraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* boxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float damage = 20.f;

public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return weaponBox; }
};
