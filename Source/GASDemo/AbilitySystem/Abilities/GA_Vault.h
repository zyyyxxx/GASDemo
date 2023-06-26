// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GD_GameplayAbility.h"
#include "GA_Vault.generated.h"

/**
 * 
 */


UCLASS()
class GASDEMO_API UGA_Vault : public UGD_GameplayAbility
{
	GENERATED_BODY()

	UGA_Vault();

	/** The last chance to fail before committing, this will usually be the same as CanActivateAbility.
	 *  Some abilities may need to do extra checks here if they are consuming extra stuff in CommitExecute */
	virtual bool CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags) override;

	//virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	//virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


protected:
	UPROPERTY(EditDefaultsOnly , Category= HorizontalTrace)
	float HorizontalTraceRadius = 30.f;

	UPROPERTY(EditDefaultsOnly , Category= HorizontalTrace)
	float HorizontalTraceLength = 500.f;

	UPROPERTY(EditDefaultsOnly , Category= HorizontalTrace)
	float HorizontalTraceCount = 5.f;

	UPROPERTY(EditDefaultsOnly , Category= HorizontalTrace)
	float HorizontalTraceStep = 30.f;
	

	UPROPERTY(EditDefaultsOnly , Category= VerticalTrace)
	float VerticalTraceRadius = 30.f;

	UPROPERTY(EditDefaultsOnly , Category= VerticalTrace)
	float VerticalTraceStep = 30.f;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* VaultMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	class UAbilityTask_PlayMontageAndWait* MontageTask = nullptr;
	
	FVector JumpToLocation;
	FVector JumpOverLocation;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannelsToIgnore;
};
