// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/GD_AnimInstance.h"

#include "ActorComponents/GD_CharacterMovementComponent.h"
#include "ActorComponents/InventoryComponent.h"
#include "Animation/AnimSequenceBase.h"
#include "Character/GD_CharacterBase.h"
#include "Animation/BlendSpace.h"
#include "DataAssets/CharacterDataAsset.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "Inventory/InventoryItemInstance.h"
#include "Kismet/KismetMathLibrary.h"

void UGD_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	GD_Character = Cast<AGD_CharacterBase>(TryGetPawnOwner());
	if(GD_Character)
	{
		GDMovementComponent = GD_Character->GetGD_CharacterMovementComponent();
	}
	
}

void UGD_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(!GD_Character || !GDMovementComponent) return;

	GetGroundSpeed();
	GetAirSpeed();
	GetShouldMove();
	GetIsFalling();
	GetIsClimbing();
	GetClimbVelocity();
	
}

void UGD_AnimInstance::GetGroundSpeed()
{
	GroundSpeed = UKismetMathLibrary::VSizeXY(GD_Character->GetVelocity());
}

void UGD_AnimInstance::GetAirSpeed()
{
	AirSpeed = GD_Character->GetVelocity().Z;
}

void UGD_AnimInstance::GetShouldMove()
{
	bShouldMove = GDMovementComponent->GetCurrentAcceleration().Size() > 0 && GroundSpeed > 5.f && !bIsFalling;
}

void UGD_AnimInstance::GetIsFalling()
{
	bIsFalling = GDMovementComponent->IsFalling();
}


void UGD_AnimInstance::GetIsClimbing()
{
	bIsClimbing = GDMovementComponent->IsClimbing(); 
}


void UGD_AnimInstance::GetClimbVelocity()
{
	ClimbVelocity = GDMovementComponent->GetUnrotatedClimbVelocity();
}

const UItemStaticData* UGD_AnimInstance::GetEquippedItemData() const
{
	AGD_CharacterBase* Character = Cast<AGD_CharacterBase>(GetOwningActor());
	UInventoryComponent* InventoryComponent = Character ? Character->GetInventoryComponent() : nullptr;
	UInventoryItemInstance* ItemInstance = InventoryComponent ? InventoryComponent->GetEquippedItem() : nullptr;

	return ItemInstance ? ItemInstance->GetItemStaticData() : nullptr;
}

UBlendSpace* UGD_AnimInstance::GetLocomotionBlendSpace() const
{
	if(AGD_CharacterBase*  GDCharacter = Cast<AGD_CharacterBase>(GetOwningActor()))
	{

		if(const UItemStaticData* ItemStaticData = GetEquippedItemData())
		{
			if(ItemStaticData->CharacterAnimationData.MovementBlendSpace)
			{
				return ItemStaticData->CharacterAnimationData.MovementBlendSpace;
			}
		}
		
		FCharacterData Data = GDCharacter->GetCharacterData();

		if(Data.CharacterAnimDataAsset)
		{
			// 获取角色Data中的AnimDataAsset后获取Asset中的CharacterAnimationData中存储的运动混合空间
			return Data.CharacterAnimDataAsset->CharacterAnimationData.MovementBlendSpace;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.MovementBlendSpace : nullptr;
}

UAnimSequenceBase* UGD_AnimInstance::GetIdleAnimation() const
{
	if(AGD_CharacterBase*  GDCharacter = Cast<AGD_CharacterBase>(GetOwningActor()))
	{

		if(const UItemStaticData* ItemStaticData = GetEquippedItemData())
		{
			if(ItemStaticData->CharacterAnimationData.IdleAnimationAsset)
			{
				return ItemStaticData->CharacterAnimationData.IdleAnimationAsset;
			} 
		}
		
		FCharacterData Data = GDCharacter->GetCharacterData();

		if(Data.CharacterAnimDataAsset)
		{
			// 获取角色Data中的AnimDataAsset后获取Asset中的CharacterAnimationData中存储的运动混合空间
			return Data.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset : nullptr;
}

UBlendSpace* UGD_AnimInstance::GetCrouchLocomotionBlendSpace() const
{
	if(AGD_CharacterBase*  GDCharacter = Cast<AGD_CharacterBase>(GetOwningActor()))
	{

		if(const UItemStaticData* ItemStaticData = GetEquippedItemData())
		{
			if(ItemStaticData->CharacterAnimationData.CrouchMovementBlendSpace)
			{
				return ItemStaticData->CharacterAnimationData.CrouchMovementBlendSpace;
			}
		}
		
		FCharacterData Data = GDCharacter->GetCharacterData();

		if(Data.CharacterAnimDataAsset)
		{
			// 获取角色Data中的AnimDataAsset后获取Asset中的CharacterAnimationData中存储的运动混合空间
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendSpace;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendSpace : nullptr;
}

UAnimSequenceBase* UGD_AnimInstance::GetCrouchIdleAnimation() const
{
	if(AGD_CharacterBase*  GDCharacter = Cast<AGD_CharacterBase>(GetOwningActor()))
	{

		if(const UItemStaticData* ItemStaticData = GetEquippedItemData())
		{
			if(ItemStaticData->CharacterAnimationData.CrouchIdleAnimationAsset)
			{
				return ItemStaticData->CharacterAnimationData.CrouchIdleAnimationAsset;
			}
		}
		
		FCharacterData Data = GDCharacter->GetCharacterData();

		if(Data.CharacterAnimDataAsset)
		{
			// 获取角色Data中的AnimDataAsset后获取Asset中的CharacterAnimationData中存储的运动混合空间
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset : nullptr;
}
