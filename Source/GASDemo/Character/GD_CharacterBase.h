// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemInterface.h"
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
	
public:
	AGD_CharacterBase();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect , FGameplayEffectContextHandle InEffectContext);
protected:

	void InitializeAttributes();//角色初始化Attribute
	void GiveAbilities(); //角色给予Ability
	void ApplyStartupEffects();

	virtual void PossessedBy(AController* NewController) override; //client
	virtual void OnRep_PlayerState() override; //server

	UPROPERTY(BlueprintReadOnly , EditDefaultsOnly , Category = "GAS")
	TSubclassOf<UGameplayEffect> DefaultAttributeSet; // 默认Attribute属性 注意是GE，我们不应该直接修改Attribute

	UPROPERTY(BlueprintReadOnly , EditDefaultsOnly , Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;  
	
	UPROPERTY(BlueprintReadOnly , EditDefaultsOnly , Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;
	
	UPROPERTY(EditDefaultsOnly)
	UGD_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(Transient)
	UGD_AttributeSet* AttributeSet;
	
	
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

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

};
