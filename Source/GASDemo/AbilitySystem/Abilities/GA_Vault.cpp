// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Vault.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActorComponents/GD_MotionWarpingComponent.h"		
#include "Character/GD_CharacterBase.h"
#include "Abilities/Gameplayability.h"

UGA_Vault::UGA_Vault()
{
	/*
	 *	How the ability is instanced when executed. This limits what an ability can do in its implementation. For example, a NonInstanced
	 *	Ability cannot have state. It is probably unsafe for an InstancedPerActor ability to have latent actions, etc.
	enum Type
	{
		// This ability is never instanced. Anything that executes the ability is operating on the CDO.
		NonInstanced,

		// Each actor gets their own instance of this ability. State can be saved, replication is possible.
		InstancedPerActor,

		// We instance this ability each time it is executed. Replication possible but not recommended.
		InstancedPerExecution,
	};
	 */
	
	// How does an ability execute on the network. Does a client "ask and predict", "ask and wait", "don't ask (just do it)".
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	// How the ability is instanced when executed. This limits what an ability can do in its implementation. 
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
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

	TArray<AActor*> ActorToIgnore = {Character};

	// Debug 
	static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugTraversal"));
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
	
	const float VerticalTraceLength = FMath::Abs(JumpToLocation.Z - (StartLocation + i * UpVector * HorizontalTraceStep).Z);


	FVector VerticalStartLocation = JumpToLocation * UpVector * VerticalTraceLength;

	i = 0;

	const float VerticalTraceCount = MaxVerticalTraceDistance / VerticalTraceStep;

	bool bJumpOverLocationSet = false;

	for(; i <= VerticalTraceCount ; ++i)
	{
		const FVector TraceStart = VerticalStartLocation + i * ForwardVector * VerticalTraceStep;
		const FVector TraceEnd = TraceStart + UpVector * VerticalTraceLength * -1;
		if(UKismetSystemLibrary::SphereTraceSingleForObjects(this , TraceStart , TraceEnd , HorizontalTraceRadius , TraceObjectTypes ,
			true , ActorToIgnore , DebugDrawType , TraceHit , true ))
		{
			JumpOverLocation = TraceHit.ImpactPoint; //比Location精确一点

			if(i == 0)
			{
				JumpToLocation = JumpOverLocation;
			}
		}else if( i != 0)
		{
			bJumpOverLocationSet = true;
			break;
		}
	}

	if(!bJumpOverLocationSet)
	{
		return false;
	}

	const FVector TraceStart = JumpOverLocation + ForwardVector * VerticalTraceStep;

	if(UKismetSystemLibrary::SphereTraceSingleForObjects(this , TraceStart , JumpOverLocation , HorizontalTraceRadius , TraceObjectTypes ,
			true , ActorToIgnore , DebugDrawType , TraceHit , true ))
	{
		JumpOverLocation = TraceHit.ImpactPoint;
	}

	if(bShowTraversal)
	{
		DrawDebugSphere(GetWorld() , JumpToLocation , 15 , 16 , FColor::White , false , 7);
		DrawDebugSphere(GetWorld() , JumpOverLocation , 15 , 16 , FColor::White , false , 7);
	}

	return true;
	
}

void UGA_Vault::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(!CommitAbility(Handle , ActorInfo , ActivationInfo))
	{
		K2_EndAbility(); //Call from blueprints to forcibly end the ability without canceling it. This will replicate the end ability to the client or server which can interrupt tasks
		return;
	}

	AGD_CharacterBase* Character = GetGD_CharacterFromActorInfo();

	UCharacterMovementComponent* CharacterMovement = Character ? Character->GetCharacterMovement() : nullptr;

	if(CharacterMovement)
	{
		CharacterMovement->SetMovementMode(MOVE_Flying);//Flying, ignoring the effects of gravity. Affected by the current physics volume's fluid friction
	}
	UCapsuleComponent* CapsuleComponent = Character ? Character->GetCapsuleComponent() : nullptr;

	if(CapsuleComponent)
	{
		for(ECollisionChannel Channel : CollisionChannelsToIgnore)
		{
			CapsuleComponent->SetCollisionResponseToChannel(Channel , ECR_Ignore);
		}
	}

	UGD_MotionWarpingComponent* MotionWarpingComponent = Character ? Character->GetGDMotionWarpingComponent() : nullptr;

	if(MotionWarpingComponent)
	{
		//进行 root motion warping
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("JumpTOLocation") , JumpToLocation , Character->GetActorRotation());
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("JumpOverLocation") , JumpOverLocation , Character->GetActorRotation());
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this , NAME_None , VaultMontage);
	MontageTask->OnBlendOut.AddDynamic(this , &UGA_Vault::K2_EndAbility);
	MontageTask->OnCompleted.AddDynamic(this , &UGA_Vault::K2_EndAbility);
	MontageTask->OnInterrupted.AddDynamic(this , &UGA_Vault::K2_EndAbility);
	MontageTask->OnCancelled.AddDynamic(this , &UGA_Vault::K2_EndAbility);
	MontageTask->ReadyForActivation();
	
}

void UGA_Vault::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if(IsValid(MontageTask))
	{
		MontageTask->EndTask();
	}
	AGD_CharacterBase* Character = GetGD_CharacterFromActorInfo();

	UCapsuleComponent* CapsuleComponent = Character ? Character->GetCapsuleComponent() : nullptr;

	if(CapsuleComponent)
	{
		for(ECollisionChannel Channel : CollisionChannelsToIgnore)
		{
			CapsuleComponent->SetCollisionResponseToChannel(Channel , ECR_Block);
		}
	}

	UCharacterMovementComponent* CharacterMovement = Character ? Character->GetCharacterMovement() : nullptr;

	if(CharacterMovement && CharacterMovement->IsFlying())
	{
		CharacterMovement->SetMovementMode(MOVE_Falling);
	}

	UGD_MotionWarpingComponent* MotionWarpingComponent = Character ? Character->GetGDMotionWarpingComponent() : nullptr;

	if(MotionWarpingComponent)
	{
		MotionWarpingComponent->RemoveWarpTarget(TEXT("JumpToLocation"));
		MotionWarpingComponent->RemoveWarpTarget(TEXT("JumpOverLocation"));
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


