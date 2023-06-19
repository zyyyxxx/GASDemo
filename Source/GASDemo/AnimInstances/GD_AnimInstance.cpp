// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/GD_AnimInstance.h"

#include "Animation/AnimSequenceBase.h"
#include "Character/GD_CharacterBase.h"
#include "Animation/BlendSpace.h"
#include "DataAssets/CharacterDataAsset.h"
#include "DataAssets/CharacterAnimDataAsset.h"

UBlendSpace* UGD_AnimInstance::GetLocomotionBlendSpace() const
{
	if(AGD_CharacterBase*  GDCharacter = Cast<AGD_CharacterBase>(GetOwningActor()))
	{
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
		FCharacterData Data = GDCharacter->GetCharacterData();

		if(Data.CharacterAnimDataAsset)
		{
			// 获取角色Data中的AnimDataAsset后获取Asset中的CharacterAnimationData中存储的运动混合空间
			return Data.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset : nullptr;
}
