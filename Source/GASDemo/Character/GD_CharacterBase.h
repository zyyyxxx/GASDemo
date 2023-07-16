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

class UGD_MotionWarpingComponent;
class UGD_CharacterMovementComponent;

class UInventoryComponent;

UCLASS(config=Game)
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

	/** Inventory Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DropItemAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* EquipNextAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* UnequipAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AttackAction;

	/** Aiming Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AimAction;
	
public:
	AGD_CharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual void PostLoad() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect , FGameplayEffectContextHandle InEffectContext);

	virtual void Landed(const FHitResult& Hit) override;

	// 重载Crouch的功能
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	UGD_MotionWarpingComponent* GetGDMotionWarpingComponent() const;

	UInventoryComponent* GetInventoryComponent() const;

	// 死亡开启布娃娃模式
	void StartRagDoll();
	
protected:


	void GiveAbilities(); //角色给予Ability
	void ApplyStartupEffects();

	virtual void PossessedBy(AController* NewController) override; //client
	virtual void OnRep_PlayerState() override; //server

	
	
	UPROPERTY(EditDefaultsOnly)
	UGD_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(Transient)
	UGD_AttributeSet* AttributeSet;

	UPROPERTY(VisibleAnywhere , BlueprintReadOnly , Category= MotionWarp)
	UGD_MotionWarpingComponent* GDMotionWarpingComponent;

	UPROPERTY()
	UGD_CharacterMovementComponent* GDCharacterMovementComponent;

	// 变为布娃娃状态回调函数
	UFUNCTION()
	void OnRagdollStateTagChanged(const FGameplayTag CallbackTag , int32 NewCount);
	
protected:
	
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

	void OnDropItemTriggered(const FInputActionValue& Value);
	void OnEquipNextTriggered(const FInputActionValue& Value);
	void OnUnequipTriggered(const FInputActionValue& Value);

	/** Called for attack input */
	void OnAttackStarted(const FInputActionValue& Value);
	void OnAttackEnded(const FInputActionValue& Value);

	/** Called for aiming */
	void OnAimStarted(const FInputActionValue& Value);
	void OnAimEnded(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void ServerProxySendGameplayEventToActor(AActor* TargetActor, FGameplayTag Tag, FGameplayEventData EventPayload);
	
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

	// 最大移动速度Attribute改变的回调函数
	void OnMaxMovementChanged(const FOnAttributeChangeData& Data);

	// 生命值Attribute改变的回调函数
	void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);

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

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AttackStartedEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AttackEndedEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AimStartedEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AimEndedEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ZeroHealthEventTag;

	// Gameplay Tags
protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer InAirTags;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer CrouchTags;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer SprintTags;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag RagdollStateTag;

	// Gameplay Effects
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CrouchStateEffect;

	// Delegates
protected:
	//FDelegateHandle MaxMovementSpeedChangedDelegateHandle;


	//Inventory
protected:
	UPROPERTY(EditAnywhere , Replicated )
	UInventoryComponent* InventoryComponent = nullptr;
	
};
