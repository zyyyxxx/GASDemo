// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameTypes.generated.h"

USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly , EditAnywhere , Category = "GAS")
	TArray<TSubclassOf<class UGameplayEffect>> Effects;

	UPROPERTY(BlueprintReadOnly , EditAnywhere , Category = "GAS")
	TArray<TSubclassOf<class UGameplayAbility>> Abilities;
};
