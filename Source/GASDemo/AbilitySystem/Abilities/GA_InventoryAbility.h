// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GD_GameplayAbility.h"
#include "GA_InventoryAbility.generated.h"

class AWeaponItemActor;
class UWeaponStaticData;
class UInventoryComponent;
class UInventoryItemInstance;
class UItemStaticData;
class AItemActor;

UCLASS()
class GASDEMO_API UGA_InventoryAbility : public UGD_GameplayAbility
{
	GENERATED_BODY()
public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintPure)
	UInventoryComponent* GetInventoryComponent() const;

	UFUNCTION(BlueprintPure)
	UInventoryItemInstance* GetEquippedItemInstance() const;

	UFUNCTION(BlueprintPure)
	const UItemStaticData* GetEquippedItemStaticData() const;

	UFUNCTION(BlueprintPure)
	const UWeaponStaticData* GetEquippedWItemWeaponStaticData() const;

	UFUNCTION(BlueprintPure)
	AItemActor* GetEquippedItemActor() const;

	UFUNCTION(BlueprintPure)
	AWeaponItemActor* GetEquippedWeaponItemActor() const;

protected:

	UPROPERTY()
	UInventoryComponent* InventoryComponent = nullptr;
};
