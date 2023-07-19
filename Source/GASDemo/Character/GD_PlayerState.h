// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "GameplayEffectTypes.h"
#include "GD_PlayerState.generated.h"


class UGD_AbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGSOnGameplayAttributeValueChangedDelegate, FGameplayAttribute,
                                               Attribute, float, NewValue, float, OldValue);

/**
 * 
 */
UCLASS()
class GASDEMO_API AGD_PlayerState : public APlayerState , public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AGD_PlayerState();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UGD_AttributeSet* GetAttributeSet() const;

	
	/**
	* Getters for attributes from GDAttributeSetBase. Returns Current Value unless otherwise specified.
	*/

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|Attributes")
	float GetMaxHealth() const;

protected:

	UPROPERTY()
	class UGD_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UGD_AttributeSet* AttributeSet;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	// Attribute changed delegate handles
	FDelegateHandle HealthChangedDelegateHandle;

	
	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	
};
