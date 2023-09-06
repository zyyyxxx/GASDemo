// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GD_AnimInstance.generated.h"

class UGD_CharacterMovementComponent;
class AGD_CharacterBase;
class UItemStaticData;

UCLASS()
class GASDEMO_API UGD_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	//Native initialization override point
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	
protected:

	// climb-------------------------------
	// 使用c++更新数据，不使用蓝图Event Graph Update Animation
	UPROPERTY()
	AGD_CharacterBase* GD_Character;

	UPROPERTY()
	UGD_CharacterMovementComponent* GDMovementComponent;
	
private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed;
	void GetGroundSpeed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float AirSpeed;
	void GetAirSpeed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float bShouldMove;
	void GetShouldMove();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float bIsFalling;
	void GetIsFalling();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	bool bIsClimbing;
	void GetIsClimbing();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	FVector ClimbVelocity;
	void GetClimbVelocity();
	//-------------------------------------

	
protected:
	const UItemStaticData* GetEquippedItemData() const;

	UFUNCTION(BlueprintCallable , meta = (BlueprintThreadSafe))
	class UBlendSpace* GetLocomotionBlendSpace() const;
	
	UFUNCTION(BlueprintCallable , meta = (BlueprintThreadSafe))
	class UAnimSequenceBase* GetIdleAnimation() const;
	
	UFUNCTION(BlueprintCallable , meta = (BlueprintThreadSafe))
	class UBlendSpace* GetCrouchLocomotionBlendSpace() const;
	
	UFUNCTION(BlueprintCallable , meta = (BlueprintThreadSafe))
	class UAnimSequenceBase* GetCrouchIdleAnimation() const;

	UPROPERTY(BlueprintReadOnly , EditAnywhere , Category= "Animation")
	class UCharacterAnimDataAsset* DefaultCharacterAnimDataAsset;
};

