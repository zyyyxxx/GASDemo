// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilityTask_TickWallRun.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWallRunWallSideDeterminedDelegate , bool , bLeftSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallRunFinishedDelegate);

class ACharacter;
class UCharacterMovementComponent;

UCLASS()
class GASDEMO_API UAbilityTask_TickWallRun : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnWallRunFinishedDelegate OnFinished;
	
	UPROPERTY(BlueprintAssignable)
	FOnWallRunWallSideDeterminedDelegate OnWallSideDetermined;

	//单例
	UFUNCTION(BlueprintCallable , Category= "Ability|Tasks" , meta = (HiddenPin = "OwningAbility" , DefaultToSelf = "OwningAbility"))
	static UAbilityTask_TickWallRun* CreatWallRunTask(UGameplayAbility* OwningAbility , ACharacter* InCharacter ,
		UCharacterMovementComponent* InCharacterMovement , TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes);

	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	virtual void TickTask(float DeltaTime) override;

protected:
	UCharacterMovementComponent* CharacterMovement = nullptr;

	ACharacter* CharacterOwner = nullptr;

	TArray<TEnumAsByte<EObjectTypeQuery>> WallRun_TraceObjectTypes;
	// 是否有可以跑的墙
	bool FindRunableWall(FHitResult& OnWallHit);
	// 选择方向
	bool IsWallOnTheLeft(const FHitResult& InWallHit) const;

	
	
};
