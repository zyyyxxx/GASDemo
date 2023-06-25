// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemInterface.h"
#include "GameTypes.h"
#include "GD_CharacterBase.generated.h"

class UGD_AbilitySystemComponent;
class UGD_AttributeSet;

class UGameplayEffect;
class UGameplayAbility;

UCLASS()
class GASDEMO_API AGD_CharacterBase : public ACharacter , public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;
	
public:
	AGD_CharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect , FGameplayEffectContextHandle InEffectContext);

	virtual void Landed(const FHitResult& Hit) override;

	// 重载Crouch的功能
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	
protected:


	void GiveAbilities(); //角色给予Ability
	void ApplyStartupEffects();

	virtual void PossessedBy(AController* NewController) override; //client
	virtual void OnRep_PlayerState() override; //server

	
	
	UPROPERTY(EditDefaultsOnly)
	UGD_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(Transient)
	UGD_AttributeSet* AttributeSet;

	
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	
	/** Called for jump input */
	void OnJumpStarted(const FInputActionValue& Value);
	void OnJumpEnded(const FInputActionValue& Value);

	/** Called for crouch input */
	void OnCrouchStarted(const FInputActionValue& Value);
	void OnCrouchEnded(const FInputActionValue& Value);

	/** Called for sprint input */
	void OnSprintStarted(const FInputActionValue& Value);
	void OnSprintEnded(const FInputActionValue& Value);	

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	// FCharacterData 接口函数
	UFUNCTION(BlueprintCallable)
	FCharacterData GetCharacterData() const;

	UFUNCTION(BlueprintCallable)
	void SetCharacterData(const FCharacterData& InCharacterData);

	class UFootstepsComponent* GetFootstepComponent() const;

	void OnMaxMovementChanged(const FOnAttributeChangeData& Data);

protected:
	
	UPROPERTY(ReplicatedUsing = OnRep_CharacterData)
	FCharacterData CharacterData;

	UFUNCTION()
	void OnRep_CharacterData();

	virtual void InitFromCharacterData(const FCharacterData& InCharacterData , bool bFromReplication = false);

	UPROPERTY(EditDefaultsOnly)
	class UCharacterDataAsset* CharacterDataAsset;

	UPROPERTY(BlueprintReadOnly)
	class UFootstepsComponent* FootstepsComponent;


	// Gameplay Events
protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag JumpEventTag;

	// Gameplay Tags
protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer InAirTags;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer CrouchTags;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer SprintTags;

	// Gameplay Effects
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CrouchStateEffect;

	// Delegates
protected:
	FDelegateHandle MaxMovementSpeedChangedDelegateHandle;
};
