// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_InventoryCombatAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Character/GD_CharacterBase.h"

FGameplayEffectSpecHandle UGA_InventoryCombatAbility::GetWeaponEffectSpec(const FHitResult& HitResult)
{
	if(UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
	{
		if(const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

			FGameplayEffectSpecHandle OutSpec = AbilitySystemComponent->MakeOutgoingSpec(
				WeaponStaticData->DamageEffect , 1 , EffectContext);

			//Sets a gameplay tag Set By Caller magnitude value
			// 使用Tag caller modifier 来 修改 health（GE的蓝图中 Modifier Magnitude 设置为Set by Caller）
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(OutSpec ,
				FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")) , -WeaponStaticData->BaseDamage);

			return OutSpec;
		}
	}
	return FGameplayEffectSpecHandle();
}
