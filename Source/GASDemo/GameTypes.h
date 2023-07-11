// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameTypes.generated.h"

class AItemActor;
class UGameplayAbility;
class UGameplayEffect;
class UAnimMontage;

USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly , EditAnywhere , Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> Effects;

	UPROPERTY(BlueprintReadOnly , EditAnywhere , Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadOnly , EditAnywhere , Category = "Animation")
	class UCharacterAnimDataAsset* CharacterAnimDataAsset;
};

USTRUCT(BlueprintType)
struct FCharacterAnimationData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	class UBlendSpace* MovementBlendSpace = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UAnimSequenceBase* IdleAnimationAsset = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UBlendSpace* CrouchMovementBlendSpace = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UAnimSequenceBase* CrouchIdleAnimationAsset = nullptr;
	
};

UENUM(BlueprintType)
enum class EFoot:uint8
{
	Left  UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
};


USTRUCT(BlueprintType)
struct FMotionWarpingTargetByLocationAndRotaion
{
	GENERATED_USTRUCT_BODY()

	FMotionWarpingTargetByLocationAndRotaion()
	{
		
	}

	FMotionWarpingTargetByLocationAndRotaion(FName InName , FVector InLocation , FRotator InRotation)
		:Name(InName) , Location(InLocation) , Rotation(InRotation)
	{
		
	}

	UPROPERTY()
	FName Name;

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;
};

UCLASS(Blueprintable , BlueprintType)
class UItemStaticData : public UObject
{

	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FName Name;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<AItemActor> ItemActorClass;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FName AttachmentSocket = NAME_None;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	bool bCanEquipped = false;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly)
	FCharacterAnimationData CharacterAnimationData;

	// Item授予的GA
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	// Item的GE
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffects;
};

UCLASS(Blueprintable , BlueprintType)
class UWeaponStaticData : public UItemStaticData
{

	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly)
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly)
	UStaticMesh* StaticMesh;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly)
	float FireRate;
};

UENUM(BlueprintType)
enum class EItemState: uint8
{
	None  UMETA(DisplayName = "None"),
	Equipped UMETA(DisplayName = "Equipped"),
	Dropped UMETA(DisplayName = "Dropped"),
};


UENUM(BlueprintType)
enum class EMovementDirectionType: uint8
{ 
	None  UMETA(DisplayName = "None"),
	OrientToMovement UMETA(DisplayName = "OrientToMovement"), // 不跟随controller旋转
	Strafe UMETA(DisplayName = "Strafe"), // 扫射 跟随controler旋转
};

