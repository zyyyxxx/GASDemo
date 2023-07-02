// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_WallRun.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_TickWallRun.h"
#include "Character/GD_CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_WallRun::UGA_WallRun()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_WallRun::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	ACharacter* CharacterAvatar = Cast<ACharacter>(ActorInfo->AvatarActor);
	UCapsuleComponent* CapsuleComponent = CharacterAvatar->GetCapsuleComponent();
	
	CapsuleComponent->OnComponentHit.AddDynamic(this, &UGA_WallRun::OnCapsuleComponentHit); //添加碰撞检测的函数
	
}

void UGA_WallRun::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if(ActorInfo)
	{
		ACharacter* CharacterAvatar = Cast<ACharacter>(ActorInfo->AvatarActor);
		UCapsuleComponent* CapsuleComponent = CharacterAvatar->GetCapsuleComponent();
		
		CapsuleComponent->OnComponentHit.RemoveDynamic(this, &UGA_WallRun::OnCapsuleComponentHit); //移除碰撞检测的函数
	}
	
	Super::OnRemoveAbility(ActorInfo, Spec);
}

bool UGA_WallRun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	AGD_CharacterBase* CharacterAvatar = GetGD_CharacterFromActorInfo();
	
	return CharacterAvatar && !CharacterAvatar->GetCharacterMovement()->IsMovingOnGround(); //在地上才可触发
}

void UGA_WallRun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//设置task 
	WallRunTickTask = UAbilityTask_TickWallRun::CreatWallRunTask(this , Cast<ACharacter>(GetAvatarActorFromActorInfo()) ,
		Cast<UCharacterMovementComponent>(ActorInfo->MovementComponent) , WallRun_TraceObjectTypes);

	WallRunTickTask->OnFinished.AddDynamic(this , &UGA_WallRun::K2_EndAbility); // 添加到task的委托 ， TasK End的时候 GA也End
	WallRunTickTask->OnWallSideDetermined.AddDynamic(this , &UGA_WallRun::OnWallSideDetermined); //添加到task的委托

	WallRunTickTask->ReadyForActivation(); //启动 Called to trigger the actual task once the delegates have been set up
}

void UGA_WallRun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if(IsValid(WallRunTickTask))
	{
		WallRunTickTask->EndTask();
	}
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(WallRunLeftSideEffectClass , AbilitySystemComponent); // 移除GE
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(WallRunRightSideEffectClass , AbilitySystemComponent); // 移除GE
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
}

void UGA_WallRun::OnCapsuleComponentHit(UPrimitiveComponent* HitCompoennt, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbility(GetCurrentAbilitySpec()->Handle); // 尝试触发当前GA ->  ActivateAbility -> 触发Task
	}
	
}

void UGA_WallRun::OnWallSideDetermined(bool bLeftSide)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	AGD_CharacterBase* CharacterAvatar = GetGD_CharacterFromActorInfo();
	// 使用GE来修改在哪一侧墙上
	FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();

	if(bLeftSide)
	{
		CharacterAvatar->ApplyGameplayEffectToSelf(WallRunLeftSideEffectClass , EffectContextHandle); // 于蓝图中选择
	}else
	{
		CharacterAvatar->ApplyGameplayEffectToSelf(WallRunRightSideEffectClass , EffectContextHandle);// 于蓝图中选择
	}
}



