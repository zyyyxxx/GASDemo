// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GD_PlayerController.generated.h"

class AGD_CharacterBase;
/**
 * 
 */
UCLASS()
class GASDEMO_API AGD_PlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// UI
	
	void CreateHUD();

	class UGD_HUDWidget* GetGDHUD();

	
	// 由GameMode负责触发
	void RestartPlayerIn(float InTime);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void OnUnPossess() override;

	virtual void BeginPlay() override;

	// 死亡回调函数
	UFUNCTION()
	void OnPawnDeathStateChanged(const FGameplayTag CallbackTag , int32 NewCount);

	void RestartPlayer();

	FTimerHandle RestartPlayerTimerHandle;

	FDelegateHandle DeathStateTagDelegate;
	
	// UI
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASShooter|UI")
	TSubclassOf<class UGD_HUDWidget> UIHUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "GASShooter|UI")
	class UGD_HUDWidget* UIHUDWidget;
};
