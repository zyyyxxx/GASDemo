// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GD_PlayerState.h"

#include "AbilitySystem/Attributes/GD_AttributeSet.h"
#include "AbilitySystem/Components/GD_AbilitySystemComponent.h"

AGD_PlayerState::AGD_PlayerState()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UGD_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSet = CreateDefaultSubobject<UGD_AttributeSet>(TEXT("AttributeSete"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	NetUpdateFrequency = 200.0f;

}

UAbilitySystemComponent* AGD_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UGD_AttributeSet* AGD_PlayerState::GetAttributeSet() const
{
	return AttributeSet;
}



float AGD_PlayerState::GetHealth() const
{
	return AttributeSet->GetHealth();
}

float AGD_PlayerState::GetMaxHealth() const
{
	return AttributeSet->GetMaxHealth();
}

void AGD_PlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &AGD_PlayerState::HealthChanged);
		
	}
}

void AGD_PlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	
}