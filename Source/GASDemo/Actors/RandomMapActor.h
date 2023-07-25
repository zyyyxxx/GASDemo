// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RandomMapActor.generated.h"

UCLASS()
class GASDEMO_API ARandomMapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARandomMapActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



};
