// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"
#include "AbilitySystemPhysicsVolume.generated.h"

struct FActiveGameplayEffectHandle;
struct FGameplayEffectSpecHandle;
struct FGameplayAbilitySpecHandle;
class UGameplayAbility;
struct FGameplayTag;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAbilityVolumeEnteredActorInfo
{
	GENERATED_USTRUCT_BODY()

	TArray<FGameplayAbilitySpecHandle> AppliedAbilities;

	TArray<FActiveGameplayEffectHandle> AppliedEffects;
	
};

UCLASS()
class GASDEMO_API AAbilitySystemPhysicsVolume : public APhysicsVolume
{
	GENERATED_BODY()
public:
	
	AAbilitySystemPhysicsVolume();

	virtual void ActorEnteredVolume(AActor* Other) override;

	virtual void ActorLeavingVolume(AActor* Other) override;

	virtual void Tick(float DeltaSeconds) override;
	
protected:

	// 进入的Effects
	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToApply;
	// 离开的Effects
	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> OnExitEffectsToApply;

	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	bool bDrawDebug = false;

	// 进入的事件Tags
	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	TArray<FGameplayTag> GameplayEventsToSendOnEnter;
	// 离开的事件Tags
	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	TArray<FGameplayTag> GameplayEventsToSendOnExit;

	// 进入的GA
	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayAbility>> OngoingAbilitiesToGive;
	// 永久的GA
	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayAbility>> PermanentAbilitiesToGive;

	UPROPERTY()
	TMap<AActor* , FAbilityVolumeEnteredActorInfo> EnteredActorsInfoMap;

	UFUNCTION(Server, Reliable)
	void ServerProxySendGameplayEventToActor(AActor* TargetActor, FGameplayTag Tag, FGameplayEventData EventData);
	
};
