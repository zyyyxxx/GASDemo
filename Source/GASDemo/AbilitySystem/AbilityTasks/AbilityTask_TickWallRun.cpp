// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_TickWallRun.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetArrayLibrary.h"

UAbilityTask_TickWallRun* UAbilityTask_TickWallRun::CreatWallRunTask(UGameplayAbility* OwningAbility,
	ACharacter* InCharacter, UCharacterMovementComponent* InCharacterMovement,
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes)
{
	UAbilityTask_TickWallRun* WallRunTask = NewAbilityTask<UAbilityTask_TickWallRun>(OwningAbility);
	WallRunTask->CharacterOwner = InCharacter;
	WallRunTask->CharacterMovement = InCharacterMovement;
	WallRunTask->bTickingTask = true;
	WallRunTask->WallRun_TraceObjectTypes = TraceObjectTypes;
}

void UAbilityTask_TickWallRun::Activate()
{
	Super::Activate();

	FHitResult OnWallHit;

	//const FVector CurrentAcceleration = CharacterMovement->GetCurrentAcceleration();

	if(!FindRunableWall(OnWallHit))
	{
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnFinished.Broadcast();
		}
		EndTask();
		return;
	}

	OnWallSideDetermined.Broadcast(IsWallOnTheLeft(OnWallHit));
	CharacterOwner->Landed(OnWallHit);
	CharacterOwner->SetActorLocation(OnWallHit.ImpactPoint + OnWallHit.ImpactNormal * 60.0f);
	CharacterMovement->SetMovementMode(MOVE_Flying);
	
}

void UAbilityTask_TickWallRun::OnDestroy(bool bInOwnerFinished)
{
	CharacterMovement->SetPlaneConstraintEnabled(false); //禁用平面约束

	CharacterMovement->SetMovementMode(MOVE_Falling);
	
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_TickWallRun::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	FHitResult OnWallHit;

	const FVector CurrentAcceleration = CharacterMovement->GetCurrentAcceleration();

	if(!FindRunableWall(OnWallHit))
	{
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnFinished.Broadcast();
		}

		EndTask();
		return;
	}

	FRotator DirectionRotator = IsWallOnTheLeft(OnWallHit) ? FRotator(0,-90,0) : FRotator(0, 90, 0);

	const FVector WallRunDirection = DirectionRotator.RotateVector(OnWallHit.ImpactPoint);

	CharacterMovement->Velocity = WallRunDirection * 700.f;

	CharacterMovement->Velocity.Z = CharacterMovement->GetGravityZ() * DeltaTime;

	CharacterOwner->SetActorRotation(WallRunDirection.Rotation());

	CharacterMovement->SetPlaneConstraintEnabled(true);
	CharacterMovement->SetPlaneConstraintOrigin(OnWallHit.ImpactPoint);
	CharacterMovement->SetPlaneConstraintNormal(OnWallHit.ImpactNormal);
	
	
}

bool UAbilityTask_TickWallRun::FindRunableWall(FHitResult& OnWallHit)
{
}

bool UAbilityTask_TickWallRun::IsWallOnTheLeft(const FHitResult& InWallHit) const
{
}
