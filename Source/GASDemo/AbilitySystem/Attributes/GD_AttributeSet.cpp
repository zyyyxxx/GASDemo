// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/GD_AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UGD_AttributeSet::PostGameplayEffectExecute(const  struct FGameplayEffectModCallbackData& Data)
{
	// PostGameplayEffectExecute仅在instantGE使Attribute的 BaseValue改变时触发
	Super::PostGameplayEffectExecute(Data);
	if(Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() , 0.f , GetMaxHealth()));
	}
}

void UGD_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGD_AttributeSet , Health , OldHealth);
}

void UGD_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGD_AttributeSet , MaxHealth , OldMaxHealth);
}

void UGD_AttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGD_AttributeSet , Health , COND_None , REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGD_AttributeSet , MaxHealth , COND_None , REPNOTIFY_Always);

}