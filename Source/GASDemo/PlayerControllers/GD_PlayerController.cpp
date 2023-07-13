// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllers/GD_PlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GASDemoGameMode.h"

void AGD_PlayerController::RestartPlayerIn(float InTime)
{
	ChangeState(NAME_Spectating);

	GetWorld()->GetTimerManager().SetTimer(RestartPlayerTimerHandle , this ,
		&AGD_PlayerController::RestartPlayer , InTime , false );
	
}

void AGD_PlayerController::RestartPlayer()
{
	UWorld* World = GetWorld();
	AGASDemoGameMode* GameMode = World ? Cast<AGASDemoGameMode>(World->GetAuthGameMode()) : nullptr;
	if(GameMode)
	{
		GameMode->RestartPlayer(this);
	}
}

void AGD_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn))
	{
		// 绑定死亡Tag触发的回调函数
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("State.Dead")) ,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this , &AGD_PlayerController::OnPawnDeathStateChanged);
	}
}

void AGD_PlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if(DeathStateTagDelegate.IsValid())
	{
		if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
		{
			// 解绑回调函数
			AbilitySystemComponent->UnregisterGameplayTagEvent(DeathStateTagDelegate ,
				FGameplayTag::RequestGameplayTag(TEXT("State.Dead")) , EGameplayTagEventType::NewOrRemoved);
		}
	}
}

void AGD_PlayerController::OnPawnDeathStateChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(NewCount > 0)
	{
		UWorld* World = GetWorld();

		AGASDemoGameMode* GameMode = World ? Cast<AGASDemoGameMode>(World->GetAuthGameMode()) : nullptr;
		
		if(GameMode)
		{
			GameMode->NotifyPlayerDied(this);	
		}

		// 安全起见再解绑一次
		if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
		{
			// 解绑回调函数
			AbilitySystemComponent->UnregisterGameplayTagEvent(DeathStateTagDelegate ,
				FGameplayTag::RequestGameplayTag(TEXT("State.Dead")) , EGameplayTagEventType::NewOrRemoved);
		}
	}
}

