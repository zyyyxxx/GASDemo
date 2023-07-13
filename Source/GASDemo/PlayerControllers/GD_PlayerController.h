// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GD_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GASDEMO_API AGD_PlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// 由GameMode负责触发
	void RestartPlayerIn(float InTime);

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	// 死亡回调函数
	UFUNCTION()
	void OnPawnDeathStateChanged(const FGameplayTag CallbackTag , int32 NewCount);

	void RestartPlayer();

	FTimerHandle RestartPlayerTimerHandle;

	FDelegateHandle DeathStateTagDelegate;
};
