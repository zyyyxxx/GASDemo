// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameTypes.h"
#include "MotionWarpingComponent.h"
#include "GD_MotionWarpingComponent.generated.h"

/**
 * 
 */
UCLASS()
class GASDEMO_API UGD_MotionWarpingComponent : public UMotionWarpingComponent
{
	GENERATED_BODY()
public:
	
	UGD_MotionWarpingComponent(const FObjectInitializer& ObjectInitializer);

	void SendWarpPointsToClients(FName InName , FVector InLocation , FRotator InRotation);

	UFUNCTION(NetMulticast , Reliable)
	void MulticastSyncWarpPoints(const FMotionWarpingTargetByLocationAndRotaion& Target);
	
};
