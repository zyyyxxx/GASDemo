// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Vault.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActorComponents/GD_MotionWarpingComponent.h"		
#include "Character/GD_CharacterBase.h"

UGA_Vault::UGA_Vault()
{
	// How does an ability execute on the network. Does a client "ask and predict", "ask and wait", "don't ask (just do it)".
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	// How the ability is instanced when executed. This limits what an ability can do in its implementation. 
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

bool UGA_Vault::CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
	if(!Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags))
	{
		return false;
	}

	AGD_CharacterBase* Character = GetGD_CharacterFromActorInfo();

	if(!IsValid(Character))
	{
		return false;
	}

	// 起始位置
	const FVector StartLocation = Character->GetActorLocation();
	const FVector ForwardVector = Character->GetActorForwardVector();
	const FVector UpVector = Character->GetActorUpVector();

	TArray<AActor*> ActorToIgnore {Character};

	// Debug 
	static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugTraversal"));\
	const bool bShowTraversal = CVar->GetInt() > 0;
	EDrawDebugTrace::Type DebugDrawType = bShowTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	
	bool bJumpToLocation = false;

	int32 JumpToLocationIdx = INDEX_NONE; // INDEX_NONE	= -1

	int i = 0;
	
	FHitResult TraceHit;

	float MaxJumpDistance = HorizontalTraceLength;

	// 进行水平球体检测
	for(; i < HorizontalTraceCount ; ++i)
	{
		const FVector TraceStart = StartLocation + i * UpVector * HorizontalTraceStep;
		const FVector TraceEnd = TraceStart + ForwardVector * HorizontalTraceLength;

		// 开始检测
		if(UKismetSystemLibrary::SphereTraceSingleForObjects(this , TraceStart , TraceEnd , HorizontalTraceRadius , TraceObjectTypes ,
			true , ActorToIgnore , DebugDrawType , TraceHit , true ))
		{
			if(JumpToLocationIdx == INDEX_NONE && (i< HorizontalTraceCount - 1))
			{
				//第一个边缘点
				JumpToLocationIdx = i;
				JumpToLocation = TraceHit.Location; 
			}
			else if(JumpToLocationIdx == (i-1))
			{
				MaxJumpDistance = FVector::Dist2D(TraceHit.Location , TraceStart); //Euclidean distance between two points in the XY plane (ignoring Z)
				break;
			}
		}
		else
		{
			if(JumpToLocationIdx != INDEX_NONE)
			{
				break;
			}
		}
	}

	if(JumpToLocationIdx == INDEX_NONE)
	{
		return false;
	}

	const float DistanceToJumpTo = FVector::Dist2D(StartLocation , JumpToLocation); //Euclidean distance between two points in the XY plane (ignoring Z)

	const float MaxVerticalTraceDistance = MaxJumpDistance - DistanceToJumpTo;

	if(MaxVerticalTraceDistance < 0)
	{
		return false;
	}

	if(i == HorizontalTraceCount)
	{
		 i = HorizontalTraceCount - 1;
	}
	
	//const float VerticalTraceLength = FMath::ABS(Jump)
	return true;
}


