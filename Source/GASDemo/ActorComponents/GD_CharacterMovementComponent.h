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
class UAnimInstance;
class UAnimMontage;

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

	FORCEINLINE FVector GetClimbableSurfaceNormal() const{return CurrentClimbableSurfaceNormal;}

	UFUNCTION(BlueprintCallable)
	FVector GetUnrotatedClimbVelocity() const;

	void StartClimbing();
	void StopClimbing();

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	//Constrain components of root motion velocity that may not be appropriate given the current movement mode (e.g. when falling Z may be ignored).
	//约束根运动速度的分量，这些分量在给定当前运动模式的情况下可能不合适（例如，当下降时，Z 可能会被忽略）。
	virtual FVector ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const override;

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
	
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly , Category= "Climb")
	UAnimMontage* IdleToClimbMontage;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly , Category= "Climb")
	UAnimMontage* ClimbToTopMontage;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly , Category= "Climb")
	UAnimMontage* ClimbDownLedgeMontage;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly , Category= "Climb")
	float ClimbDownWalkableSurfaceTraceOffset = 50.f;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly , Category= "Climb")
	float ClimbDownLedgeTraceOffset = 50.f;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ClimbStateStartEffect;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ClimbStateEndEffect;
	
	TArray<FHitResult> ClimbableSurfacesTraceResults;

	FVector CurrentClimbableSurfaceLocation;
	FVector CurrentClimbableSurfaceNormal;

	UPROPERTY()
	UAnimInstance* OwningPlayerAnimInstance;
	
	TArray<FHitResult> ClimbDoCapsuleTraceMultiByObject(const FVector& Start , const FVector& End ,
		bool bShowDebugShape = false , bool bDrawPersistantShape = false);
	FHitResult ClimbDoLineTraceSingleByObject(const FVector& Start , const FVector& End ,
		bool bShowDebugShape = false , bool bDrawPersistantShape = false);

	bool TraceClimbableSurfaces();
	FHitResult TraceFromEyeHeight(float TraceDistance , float TraceStartOffset = 0.f);

	void PhysClimb(float deltaTime, int32 Iterations);

	void ProcessClimbableSurfaceInfo();

	bool CheckShouldStopClimbing();

	bool CheckHasReachedFloor();
	
	FQuat GetClimbRotation(float DeltaTime);

	void SnapMovementToClimbableSurfaces(float DeltaTime);
	
	// 攀爬到顶部边缘检测
	bool CheckHasReachedLedge();

	// 顶部向下攀爬检测
	bool CanClimbDownLedge();
	
	void PlayClimbMontage(UAnimMontage* MontageToPlay);

	UFUNCTION()
	void OnClimbMontageEnded(UAnimMontage* Montage , bool bInterrupted);
	
	void ApplyClimbStartedGE();
	void ApplyClimbEndedGE();
	
#pragma endregion 

	void HandleMovementDirection();
};
