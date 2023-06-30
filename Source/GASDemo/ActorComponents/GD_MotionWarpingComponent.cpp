// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/GD_MotionWarpingComponent.h"

UGD_MotionWarpingComponent::UGD_MotionWarpingComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UGD_MotionWarpingComponent::SendWarpPointsToClients(FName InName , FVector InLocation , FRotator InRotation)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		// is server
		
		FMotionWarpingTargetByLocationAndRotaion WarpTarget(InName , InLocation , InRotation);
		

		MulticastSyncWarpPoints(WarpTarget);
	}
}

void UGD_MotionWarpingComponent::MulticastSyncWarpPoints_Implementation(const FMotionWarpingTargetByLocationAndRotaion& Target)
{
	// 打包发送
	
	AddOrUpdateWarpTargetFromLocationAndRotation(Target.Name , Target.Location , FRotator(Target.Rotation));
	
}
