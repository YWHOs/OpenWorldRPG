#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHand UMETA(DisplayName = "Equipped One Hand"),
	ECS_EquippedTwoHand UMETA(DisplayName = "Equipped Two Hand")
};
