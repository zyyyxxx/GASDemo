// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GD_GameplayAbility.h"
#include "AbilitySystemLog.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GD_AttributeSet.h"
#include "Character/GD_CharacterBase.h"

void UGD_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();

	for(auto GameplayEffect : OngoingEffectsToJustApplyOnStart)
	{
		if(!GameplayEffect.Get()) continue;

		if(UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get())
		{
			FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(GameplayEffect , 1, EffectContext);
			if(SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				if(!ActiveGEHandle.WasSuccessfullyApplied())
				{
					ABILITY_LOG(Log , TEXT("Ability %s failed to apply startup effect %s! "), *GetName(),*GetNameSafe(GameplayEffect));
				}
			}
		}
		
	}

	if(IsInstantiated())//已经实例化
	{
		for(auto GameplayEffect : OngoingEffectsToRemoveOnEnd)
		{
			if(!GameplayEffect.Get()) continue;

			if(UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get())
			{
				FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(GameplayEffect , 1, EffectContext);
				if(SpecHandle.IsValid())
				{
					FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					if(!ActiveGEHandle.WasSuccessfullyApplied())
					{
						ABILITY_LOG(Log , TEXT("Ability %s failed to apply startup effect %s! "), *GetName(),*GetNameSafe(GameplayEffect));
					}
					else
					{
						RemoveOnEndEffectHandles.Add(ActiveGEHandle);
					}
				}
			}
		}
	}
	
}

void UGD_GameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
									const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
									bool bReplicateEndAbility, bool bWasCancelled)
{
	
	if(IsInstantiated())
	{
		for(FActiveGameplayEffectHandle ActiveGEHandle : RemoveOnEndEffectHandles)
		{
			if(ActiveGEHandle.IsValid())
			{
				ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveGEHandle);
			}
		}
		RemoveOnEndEffectHandles.Empty();  //Empties the array. It calls the destructors on held items if needed.
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

AGD_CharacterBase* UGD_GameplayAbility::GetGD_CharacterFromActorInfo() const
{
	return Cast<AGD_CharacterBase>(GetAvatarActorFromActorInfo());
}
