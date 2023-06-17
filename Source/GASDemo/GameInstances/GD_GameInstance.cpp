// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstances/GD_GameInstance.h"
#include "AbilitySystemGlobals.h"

void UGD_GameInstance::Init()
{
	Super::Init();
	UAbilitySystemGlobals::Get().InitGlobalData(); //初始化GAS
}
