// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GD_GameplayAbility.h"
#include "GA_Jump.generated.h"

class UGameplayEffect;
UCLASS()
class GASDEMO_API UGA_Jump : public UGD_GameplayAbility
{
	GENERATED_BODY()

	UGA_Jump();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	/* 已经移至基类
	UPROPERTY(EditDefaultsOnly , Category = "Effects")
	TSubclassOf<UGameplayEffect> JumpEffect;
	*/

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WallRunStateTag;

	UPROPERTY(EditDefaultsOnly)
	float OffWallJumpStrength = 100.f;
};
