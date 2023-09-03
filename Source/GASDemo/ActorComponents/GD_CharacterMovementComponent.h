// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GD_CharacterMovementComponent.generated.h"

UENUM(BlueprintType)
namespace ECustomMovementMode
{
	enum Type
	{
		MOVE_Climb UMETA(DisplayName = "Climb Mode")
	};
}


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


#pragma region Climb
	 
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ToggleClimbing(bool bEnableClimb);
	
	bool IsClimbing() const;
	bool CanStartClimbing();

	void StartClimbing();
	void StopClimbing();

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> ClimbTraceObjectTypes;
	
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly , Category= "Climb")
	float ClimbCapsuleTraceRadius = 50.0f;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly , Category= "Climb")
	float ClimbCapsuleTraceHeight = 72.0f;
	
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly , Category= "Climb")
	float MaxBreakClimbDeceleration = 400.0f;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly , Category= "Climb")
	float MaxClimbSpeed = 100.0f;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly , Category= "Climb")
	float MaxClimbAcceleration = 300.0f;
	
	TArray<FHitResult> ClimbableSurfacesTraceResults;

	FVector CurrentClimbableSurfaceLocation;
	FVector CurrentClimbableSurfaceNormal;
	
	TArray<FHitResult> ClimbDoCapsuleTraceMultiByObject(const FVector& Start , const FVector& End ,
		bool bShowDebugShape = false , bool bDrawPersistantShape = false);
	FHitResult ClimbDoLineTraceSingleByObject(const FVector& Start , const FVector& End ,
		bool bShowDebugShape = false , bool bDrawPersistantShape = false);

	bool TraceClimbableSurfaces();
	FHitResult TraceFromEyeHeight(float TraceDistance , float TraceStartOffset = 0.f);

	void PhysClimb(float deltaTime, int32 Iterations);

	void ProcessClimbableSurfaceInfo();

	FQuat GetClimbRotation(float DeltaTime);

	void SnapMovementToClimbableSurfaces(float DeltaTime);
#pragma endregion 

	void HandleMovementDirection();
};
