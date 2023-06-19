// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GD_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GASDEMO_API UGD_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable , meta = (BlueprintThreadSafe))
	class UBlendSpace* GetLocomotionBlendSpace() const;

	
	UFUNCTION(BlueprintCallable , meta = (BlueprintThreadSafe))
	class UAnimSequenceBase* GetIdleAnimation() const;

	UPROPERTY(BlueprintReadOnly , EditAnywhere , Category= "Animation")
	class UCharacterAnimDataAsset* DefaultCharacterAnimDataAsset;
};

