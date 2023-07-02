// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GDGameStatics.generated.h"

class UItemStaticData;

UCLASS()
class GASDEMO_API UGDGameStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable , BlueprintPure)
	static const UItemStaticData* GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass);
};
