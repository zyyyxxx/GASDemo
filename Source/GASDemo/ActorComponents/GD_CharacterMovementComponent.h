// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GD_CharacterMovementComponent.generated.h"

struct FGameplayTag;
class UAbilitySystemComponent;
class UGameplayAbility;

UCLASS()
class GASDEMO_API UGD_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	bool TryTraversal(UAbilitySystemComponent* ASC);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	EMovementDirectionType GetMovementDirectionType() const;

	UFUNCTION(BlueprintCallable)
	void SetMovementDirectionType(EMovementDirectionType InMovementDirectionType);

	UFUNCTION()
	void OnEnforcedStrafeTagChanged(const FGameplayTag CallbackTag , int32 NewCount);

protected:

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> TraversalAbilitiesOrdered; 

	UPROPERTY(EditAnywhere)
	EMovementDirectionType MovementDirectionType;


#pragma region Climb;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> ClimbTraceObjectTypes;
	
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly , Category= "Climb")
	float ClimbCapsuleTraceRadius = 50.0f;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly , Category= "Climb")
	float ClimbCapsuleTraceHeight = 72.0f;
	
	TArray<FHitResult> ClimbDoCapsuleTraceMultiByObject(const FVector& Start , const FVector& End , bool bShowDebugShape = false);

	void TraceClimableSurfaces();
	
#pragma endregion 

	void HandleMovementDirection();
};
