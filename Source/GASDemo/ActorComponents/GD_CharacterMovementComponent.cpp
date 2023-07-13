// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/GD_CharacterMovementComponent.h"

#include <set>

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "MotionWarpingComponent.h"

static TAutoConsoleVariable<int32> CVarShowTraversal(
	TEXT("ShowDebugTraversal"),
	0,
	TEXT("Draw debug info about traversal")
	TEXT("0: off/n")
	TEXT("1: on/n"),
	ECVF_Cheat
);

bool UGD_CharacterMovementComponent::TryTraversal(UAbilitySystemComponent* ASC)
{
	for(TSubclassOf<UGameplayAbility> AbilityClass : TraversalAbilitiesOrdered)
	{
		// 按顺序尝试 Vault 和 Jump
		if(ASC->TryActivateAbilityByClass(AbilityClass , true))
		{
			FGameplayAbilitySpec* Spec;

			Spec = ASC->FindAbilitySpecFromClass(AbilityClass);
			if(Spec && Spec->IsActive())
			{
				return true;
			}
		}
	}
	return false;
}

void UGD_CharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	HandleMovementDirection();

	if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		/** Allow events to be registered for specific gameplay tags being added or removed */
		// GE_EnforceStrafe 添加 Tag ，调用回调函数
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("Movement.Enforced.Strafe")),
			EGameplayTagEventType::NewOrRemoved).
		AddUObject(this , &UGD_CharacterMovementComponent::OnEnforcedStrafeTagChanged); // 委托绑定回调函数
	}
	
}

void UGD_CharacterMovementComponent::OnEnforcedStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(NewCount)
	{
		SetMovementDirectionType(EMovementDirectionType::Strafe);
	}else
	{
		SetMovementDirectionType(EMovementDirectionType::OrientToMovement);
	}
}


void UGD_CharacterMovementComponent::HandleMovementDirection()
{
	switch(MovementDirectionType)
	{
	case EMovementDirectionType::Strafe:
		bUseControllerDesiredRotation = true;
		bOrientRotationToMovement = false;
		CharacterOwner->bUseControllerRotationYaw = true;
		break;
	

	default:
		bUseControllerDesiredRotation = false;
		bOrientRotationToMovement = true;
		CharacterOwner->bUseControllerRotationYaw = false;
		break;
		
	}
}


EMovementDirectionType UGD_CharacterMovementComponent::GetMovementDirectionType() const
{
	return MovementDirectionType;
}

void UGD_CharacterMovementComponent::SetMovementDirectionType(EMovementDirectionType InMovementDirectionType)
{
	MovementDirectionType = InMovementDirectionType;

	HandleMovementDirection();
}



