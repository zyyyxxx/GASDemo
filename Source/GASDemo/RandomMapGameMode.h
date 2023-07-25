// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASDemoGameMode.h"
#include "RandomMapGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GASDEMO_API ARandomMapGameMode : public AGASDemoGameMode
{
	GENERATED_BODY()
	
public:
	ARandomMapGameMode();

	
protected:
	
	
	UPROPERTY(EditAnywhere, Category = "Random Map")
	TSubclassOf<AActor> RandomMapActor;

	
};
