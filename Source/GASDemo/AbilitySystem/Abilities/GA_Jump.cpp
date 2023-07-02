// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Jump.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "AbilitySystemBlueprintLibrary.h"

UGA_Jump::UGA_Jump()
{
	// How does an ability execute on the network. Does a client "ask and predict", "ask and wait", "don't ask (just do it)".
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	// How the ability is instanced when executed. This limits what an ability can do in its implementation. 
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}


bool UGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get() , ECastCheckedType::NullAllowed);

	// 检测WallRun
	const bool bMomentAllowsJump = Character->GetMovementComponent()->IsJumpAllowed();
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character);
	const bool bIsWallRunning = AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(WallRunStateTag);
	
	return Character->CanJump() || (bMomentAllowsJump && bIsWallRunning);
}

void UGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if(HasAuthorityOrPredictionKey(ActorInfo , &ActivationInfo)) // True if we are authority or we have a valid prediciton key that is expected to work
	{
		if(!CommitAbility(Handle, ActorInfo , ActivationInfo)) // Commit = spend resources/cooldowns. It's possible the source has changed state since it started activation, so a commit may fail.
		{
			return;
		}

		Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
		
		ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get() , ECastCheckedType::NullAllowed);


		UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character);
		const bool bIsWallRunning = AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(WallRunStateTag);

		if(bIsWallRunning)
		{
			FGameplayTagContainer WallRunTags(WallRunStateTag);

			AbilitySystemComponent->CancelAbilities(&WallRunTags); // 此处蓝图存的是StateTag，所以WallRunGA蓝图里也要添加stateTag，才能正确cancel

			FVector JumpOffVector = Character->GetCharacterMovement()->GetCurrentAcceleration().GetSafeNormal() + FVector::UpVector;

			Character->LaunchCharacter(JumpOffVector * OffWallJumpStrength , true , true);
		}else
		{
			Character->Jump();
		}

		
		//以上是自带的JumpGA的代码

		/* 此处功能已在基类中实现
		//我们添加应用GE的功能
		//获取ASC
		if(UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get())
		{
			//获取GE Context的Handle
			FGameplayEffectContextHandle EffectContext = AbilityComponent->MakeEffectContext();
			//获取Spec的Handle
			FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(JumpEffect , 1 , EffectContext);
			if(SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				if(!ActiveGEHandle.WasSuccessfullyApplied())
				{
					UE_LOG(LogTemp , Error , TEXT("Faild to apply jump effect! %s") , *GetNameSafe(Character));
				}
			}
		}*/
	}
}
