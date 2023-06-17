// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GD_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GASDEMO_API UGD_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	virtual void Init() override; //重写Init
};
