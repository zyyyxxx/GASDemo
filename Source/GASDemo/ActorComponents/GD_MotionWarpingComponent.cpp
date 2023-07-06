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
	if(Target.Name == "JumpToLocation")
	{
		DrawDebugSphere(GetWorld() , Target.Location , 15 , 16 , FColor::Blue , false , 10);
	}else
	{
		DrawDebugSphere(GetWorld() , Target.Location , 15 , 16 , FColor::Green , false , 10);
	}
	
	AddOrUpdateWarpTargetFromLocationAndRotation(Target.Name , Target.Location , FRotator(Target.Rotation));
	
}
