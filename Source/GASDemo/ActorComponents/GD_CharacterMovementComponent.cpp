// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/GD_CharacterMovementComponent.h"

#include <set>

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "MotionWarpingComponent.h"
#include "Character/GD_CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"

static TAutoConsoleVariable<int32> CVarShowTraversal(
	TEXT("ShowDebugTraversal"),
	0,
	TEXT("Draw debug info about traversal")
	TEXT("0: off/n")
	TEXT("1: on/n"),
	ECVF_Cheat
);

void UGD_CharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// 攀爬动画相关
	OwningPlayerAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
	if(OwningPlayerAnimInstance)
	{
		OwningPlayerAnimInstance->OnMontageEnded.AddDynamic(this , &UGD_CharacterMovementComponent::OnClimbMontageEnded);
		OwningPlayerAnimInstance->OnMontageBlendingOut.AddDynamic(this , &UGD_CharacterMovementComponent::OnClimbMontageEnded);
	}

	// GAS与Strafe相关
	HandleMovementDirection();
	 
	if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		/** Allow events to be registered for specific gameplay tags being added or removed */
		// GE_EnforceStrafe 添加 Tag ，调用回调函数
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("Movement.Enforced.Strafe")),
			EGameplayTagEventType::NewOrRemoved).
		AddUObject(this , &UGD_CharacterMovementComponent::OnEnforcedStrafeTagChanged); // 委托绑定回调函数
	}
	
}


void UGD_CharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//UE_LOG(LogTemp , Warning , TEXT("%d") , MovementMode.GetValue());
	//TraceClimbableSurfaces();
	//TraceFromEyeHeight(100.f);
}

/* ---------------------Strafe------------------------*/

void UGD_CharacterMovementComponent::OnEnforcedStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(NewCount)
	{
		SetMovementDirectionType(EMovementDirectionType::Strafe);
	}else
	{
		SetMovementDirectionType(EMovementDirectionType::OrientToMovement);
	}
}

/*----------------------Climb---------------------*/
void UGD_CharacterMovementComponent::ToggleClimbing(bool bEnableClimb)
{
	
	if(bEnableClimb)
	{
		if(CanStartClimbing())
		{
			// 开启攀爬状态
			PlayClimbMontage(IdleToClimbMontage);
			//StartClimbing();
			ApplyClimbStartedGE();
			Debug::Print(TEXT("Start Climb!"));
			
		}else if(CanClimbDownLedge())
		{
			PlayClimbMontage(ClimbDownLedgeMontage);
			ApplyClimbStartedGE();
			Debug::Print(TEXT("Can Climb Down") , FColor::Cyan	,1);
			
		}else
		{
			Debug::Print(TEXT("Can Not Climb Down") , FColor::Red	,1);
		}
	}else
	{
		// 关闭攀爬
		StopClimbing();

		ApplyClimbEndedGE();
		Debug::Print(TEXT("Stop Climb!"));
	}
}

bool UGD_CharacterMovementComponent::IsClimbing() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == ECustomMovementMode::MOVE_Climb;
}

bool UGD_CharacterMovementComponent::CanStartClimbing()
{
	if(IsFalling()) return false;
	if(!TraceClimbableSurfaces()) return false;
	if(!TraceFromEyeHeight(100.f).bBlockingHit) return false;

	return true;
}

FVector UGD_CharacterMovementComponent::GetUnrotatedClimbVelocity() const
{
	return UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat() , Velocity);
}

void UGD_CharacterMovementComponent::StartClimbing()
{
	SetMovementMode(MOVE_Custom , ECustomMovementMode::MOVE_Climb);
	
}

void UGD_CharacterMovementComponent::StopClimbing()
{
	SetMovementMode(MOVE_Falling);
}

void UGD_CharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	
	if(IsClimbing())
	{
		bOrientRotationToMovement = false;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(48.f);
		
	}

	if(PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::MOVE_Climb)
	{
		bOrientRotationToMovement = true;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);

		StopMovementImmediately();
	}
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

// 开启自定义Phys更新函数
void UGD_CharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if(IsClimbing())
	{
		PhysClimb(deltaTime , Iterations);	
	}

	Super::PhysCustom(deltaTime, Iterations);

	
}

void UGD_CharacterMovementComponent::PhysClimb(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	// 获取所有的可攀爬表面信息
	TraceClimbableSurfaces();
	ProcessClimbableSurfaceInfo();

	
	
	// 检查我们是否需要停止攀爬
	if(CheckShouldStopClimbing() || CheckHasReachedFloor())
	{
		StopClimbing();
	}
	
	RestorePreAdditiveRootMotionVelocity();

	if( !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{
		// 定义最大攀爬速度和加速度
		CalcVelocity(deltaTime, 0.f, true, MaxBreakClimbDeceleration);
	}

	ApplyRootMotionToVelocity(deltaTime);

	
	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime;
	FHitResult Hit(1.f);

	// 处理攀爬Rotation ，调用自定的GetClimbRotation更改rotation
	SafeMoveUpdatedComponent(Adjusted, GetClimbRotation(deltaTime), true, Hit);

	if (Hit.Time < 1.f)
	{
		//adjust and try again
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	if(!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	// 将角色运动附着到可攀爬表面
	SnapMovementToClimbableSurfaces(deltaTime);

	if(CheckHasReachedLedge())
	{
		
		ApplyClimbEndedGE();
		PlayClimbMontage(ClimbToTopMontage);
	}
	
}

void UGD_CharacterMovementComponent::ProcessClimbableSurfaceInfo()
{
	CurrentClimbableSurfaceLocation = FVector::ZeroVector;
	CurrentClimbableSurfaceNormal = FVector::ZeroVector;

	if(ClimbableSurfacesTraceResults.IsEmpty()) return ;

	for(const auto& TraceHitResult : ClimbableSurfacesTraceResults)
	{
		CurrentClimbableSurfaceLocation += TraceHitResult.ImpactPoint;
		CurrentClimbableSurfaceNormal += TraceHitResult.ImpactNormal;
	}

	CurrentClimbableSurfaceLocation /= ClimbableSurfacesTraceResults.Num();
	CurrentClimbableSurfaceNormal = CurrentClimbableSurfaceNormal.GetSafeNormal();
}

bool UGD_CharacterMovementComponent::CheckShouldStopClimbing()
{
	if(ClimbableSurfacesTraceResults.IsEmpty()) return true;

	// 攀爬平面法线方向与垂直方向夹角
	const float DotResult = FVector::DotProduct(CurrentClimbableSurfaceNormal , FVector::UpVector);
	const float DegressDiff = FMath::RadiansToDegrees(FMath::Acos(DotResult));

	if(DegressDiff <= 60.f)
	{
		return true;
	}
	return false;
	
}

bool UGD_CharacterMovementComponent::CheckHasReachedFloor()
{
	const FVector DownVector = -UpdatedComponent->GetUpVector();
	const FVector StartOffset = DownVector * 50.f;

	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + DownVector;

	TArray<FHitResult> PossibleFloorHits =  ClimbDoCapsuleTraceMultiByObject(Start , End , true);
	if(PossibleFloorHits.IsEmpty()) return false;

	for(const auto& PossibleFloorHit:PossibleFloorHits)
	{
		// 检测到地面法线，并且在向下运动
		const bool bFloorReached = FVector::Parallel(-PossibleFloorHit.ImpactNormal , FVector::UpVector) && GetUnrotatedClimbVelocity().Z < -10.f;
		if(bFloorReached)
		{
			return true;
		}
		
	}
	return false;
}

FQuat UGD_CharacterMovementComponent::GetClimbRotation(float DeltaTime)
{
	const FQuat CurrentQuat = UpdatedComponent->GetComponentQuat();

	if(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return CurrentQuat;
	}
	const FQuat TargetQuat = FRotationMatrix::MakeFromX(-CurrentClimbableSurfaceNormal).ToQuat();
	// 插值
	return FMath::QInterpTo(CurrentQuat , TargetQuat , DeltaTime , 5.f);
}

float UGD_CharacterMovementComponent::GetMaxSpeed() const
{
	if(IsClimbing())
	{
		return MaxClimbSpeed;
	}else
	{
		return Super::GetMaxSpeed();	
	}
	
}

float UGD_CharacterMovementComponent::GetMaxAcceleration() const
{
	if(IsClimbing())
	{
		return MaxClimbAcceleration;
	}else
	{
		return Super::GetMaxAcceleration();	
	}
}

FVector UGD_CharacterMovementComponent::ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity,
	const FVector& CurrentVelocity) const
{
	const bool bIsPlayingRMMontage = IsFalling() && OwningPlayerAnimInstance && OwningPlayerAnimInstance->IsAnyMontagePlaying();
	if(bIsPlayingRMMontage)
	{
		return RootMotionVelocity;
	}else
	{
		return Super::ConstrainAnimRootMotionVelocity(RootMotionVelocity, CurrentVelocity);
	}
}


void UGD_CharacterMovementComponent::SnapMovementToClimbableSurfaces(float DeltaTime)
{
	const FVector ComponentForward = UpdatedComponent->GetForwardVector();
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();

	// 角色位置指向攀爬平面的向量 投影至角色正方向
	const FVector ProjectedCharacterToSurface = (CurrentClimbableSurfaceLocation - ComponentLocation)
		.ProjectOnTo(ComponentForward);

	// 获得攀爬平面法向量的反方向 长度设置为角色到平面的距离
	const FVector SnapVector = -CurrentClimbableSurfaceNormal * ProjectedCharacterToSurface.Length();
	
	//尝试通过移动矢量 （增量） 移动组件，并将旋转设置为 NewRotation。
	//假定组件的当前位置有效，并且组件适合其当前位置。调度阻止命中通知（如果 bSweep 为 true），
	//并在移动后调用 UpdateOverlaps（） 以更新重叠状态
	UpdatedComponent->MoveComponent(
		SnapVector * DeltaTime * MaxClimbSpeed,
		UpdatedComponent->GetComponentQuat(),
		true);
	
}

bool UGD_CharacterMovementComponent::CheckHasReachedLedge()
{
	// 水平检测是否有空间
	FHitResult LedgeHitResult = TraceFromEyeHeight(100.f  ,  50.f);

	if(!LedgeHitResult.bBlockingHit)
	{
		//竖直检测攀爬到顶部是否可站立
		const FVector WalkableSurfaceTraceStart = LedgeHitResult.TraceEnd;
		const FVector DownVector = -UpdatedComponent->GetUpVector();
		
		const FVector WalkableSurfaceTraceEnd = WalkableSurfaceTraceStart + DownVector * 100.f;
		FHitResult WalkableSurfaceHitResult = ClimbDoLineTraceSingleByObject(WalkableSurfaceTraceStart , WalkableSurfaceTraceEnd , true);

		if(WalkableSurfaceHitResult.bBlockingHit && GetUnrotatedClimbVelocity().Z > 10.f)
		{
			return true;
		}
	}
	return false;
}

bool UGD_CharacterMovementComponent::CanClimbDownLedge()
{
	if(IsFalling()) return false;
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	const FVector ForwardVector = UpdatedComponent->GetForwardVector();
	const FVector DownVector = -UpdatedComponent->GetUpVector();

	const FVector WalkableSurfaceTraceStart = ComponentLocation * ForwardVector * ClimbDownWalkableSurfaceTraceOffset;
	const FVector WalkableSurfaceTraceEnd = WalkableSurfaceTraceStart * DownVector * 100.f;

	// 较近的向下检测
	FHitResult WalkableSurfaceHit = ClimbDoLineTraceSingleByObject(WalkableSurfaceTraceStart, WalkableSurfaceTraceEnd , true);
	// 较远的向下检测
	const FVector LedgeTraceStart = WalkableSurfaceHit.TraceStart + ForwardVector * ClimbDownLedgeTraceOffset;
	const FVector LedgeTraceEnd = LedgeTraceStart + DownVector * 200.f;
	FHitResult LedgeTraceHit = ClimbDoLineTraceSingleByObject(LedgeTraceStart, LedgeTraceEnd , true);

	if(WalkableSurfaceHit.bBlockingHit && !LedgeTraceHit.bBlockingHit)
	{
		return true;
	}

	return false;
}

FHitResult UGD_CharacterMovementComponent::ClimbDoLineTraceSingleByObject(const FVector& Start, const FVector& End,
                                                                          bool bShowDebugShape,bool bDrawPersistantShape)
{
	FHitResult OutHit;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;
	if(bShowDebugShape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;
		if(bDrawPersistantShape)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}
	
	UKismetSystemLibrary::LineTraceSingleForObjects(
			this ,
			Start ,
			End ,
			ClimbTraceObjectTypes ,
			false ,
			TArray<AActor*>() ,
			DebugTraceType,
			OutHit ,
			false,
 			FColor::Blue
	);

	return OutHit;
}


TArray<FHitResult> UGD_CharacterMovementComponent::ClimbDoCapsuleTraceMultiByObject(const FVector& Start, const FVector& End,
                                                                                    bool bShowDebugShape,bool bDrawPersistantShape)
{
	TArray<FHitResult> OutCapsuleTraceHitResults;
	
	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;
	if(bShowDebugShape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;
		if(bDrawPersistantShape)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
			this ,
			Start ,
			End ,
			ClimbCapsuleTraceRadius ,
			ClimbCapsuleTraceHeight ,
			ClimbTraceObjectTypes ,
			false ,
			TArray<AActor*>() ,
			DebugTraceType ,
			OutCapsuleTraceHitResults ,
			false
	);

	return OutCapsuleTraceHitResults;
}

bool UGD_CharacterMovementComponent::TraceClimbableSurfaces()
{
	const FVector StartOffset = UpdatedComponent->GetForwardVector() * 30.f;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();
	
	ClimbableSurfacesTraceResults = ClimbDoCapsuleTraceMultiByObject(Start , End , true , false);

	return !ClimbableSurfacesTraceResults.IsEmpty();
}

FHitResult UGD_CharacterMovementComponent::TraceFromEyeHeight(float TraceDistance, float TraceStartOffset)
{
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	const FVector EyeHeightOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);

	const FVector Start = ComponentLocation + EyeHeightOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * TraceDistance;

	return ClimbDoLineTraceSingleByObject(Start, End, true , false);
}


void UGD_CharacterMovementComponent::PlayClimbMontage(UAnimMontage* MontageToPlay)
{
	if(!MontageToPlay || !OwningPlayerAnimInstance || OwningPlayerAnimInstance->IsAnyMontagePlaying()) return;
	
	OwningPlayerAnimInstance->Montage_Play(MontageToPlay);
}

void UGD_CharacterMovementComponent::OnClimbMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if(Montage == IdleToClimbMontage || Montage == ClimbDownLedgeMontage)
	{
		//蒙太奇结束 进入攀爬状态
		StartClimbing();
		//消除蒙太奇的速度	
		StopMovementImmediately();
	}
	
	if(Montage == ClimbToTopMontage){
		// 攀爬到顶部 进入步行模式
		SetMovementMode(MOVE_Walking);
	}
}

void UGD_CharacterMovementComponent::ApplyClimbStartedGE()
{
	AGD_CharacterBase* Character = Cast<AGD_CharacterBase>(GetPawnOwner());
	if(!Character) return;
	UAbilitySystemComponent* AbilitySystemComponent= Character->GetAbilitySystemComponent();

	if(AbilitySystemComponent)
	{
		//获取GE Context的Handle
		FGameplayEffectContextHandle ClimbStartEffectContext = AbilitySystemComponent->MakeEffectContext();
		//获取Spec的Handle
		FGameplayEffectSpecHandle ClimbStartSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(ClimbStateStartEffect , 1 ,
			ClimbStartEffectContext);
		FActiveGameplayEffectHandle ClimbStartActiveGEHandle;

		if(ClimbStartSpecHandle.IsValid())
		{
			ClimbStartActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*ClimbStartSpecHandle.Data.Get());
				
		}
	}
}

void UGD_CharacterMovementComponent::ApplyClimbEndedGE()
{
	AGD_CharacterBase* Character = Cast<AGD_CharacterBase>(GetPawnOwner());
	if(!Character) return;
	UAbilitySystemComponent* AbilitySystemComponent= Character->GetAbilitySystemComponent();
	
	//获取GE Context的Handle
	FGameplayEffectContextHandle ClimbEndEffectContext = AbilitySystemComponent->MakeEffectContext();
	//获取Spec的Handle
	FGameplayEffectSpecHandle ClimbEndSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(ClimbStateEndEffect , 1 ,
		ClimbEndEffectContext);
	FActiveGameplayEffectHandle ClimbEndActiveGEHandle;

	if(ClimbEndSpecHandle.IsValid())
	{
		ClimbEndActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*ClimbEndSpecHandle.Data.Get());
				
	}
}


/*------------Wall Run--------------*/

bool UGD_CharacterMovementComponent::TryTraversal(UAbilitySystemComponent* ASC)
{
	for(TSubclassOf<UGameplayAbility> AbilityClass : TraversalAbilitiesOrdered)
	{
		// 按顺序尝试 Vault 和 Jump
		if(ASC->TryActivateAbilityByClass(AbilityClass , true))
		{
			FGameplayAbilitySpec* Spec;

			Spec = ASC->FindAbilitySpecFromClass(AbilityClass);
			if(Spec && Spec->IsActive())
			{
				return true;
			}
		}
	}
	return false;
}


void UGD_CharacterMovementComponent::HandleMovementDirection()
{
	switch(MovementDirectionType)
	{
	case EMovementDirectionType::Strafe:
		bUseControllerDesiredRotation = true;
		bOrientRotationToMovement = false;
		CharacterOwner->bUseControllerRotationYaw = true;
		break;
	

	default:
		bUseControllerDesiredRotation = false;
		bOrientRotationToMovement = true;
		CharacterOwner->bUseControllerRotationYaw = false;
		break;
		
	}
}


EMovementDirectionType UGD_CharacterMovementComponent::GetMovementDirectionType() const
{
	return MovementDirectionType;
}

void UGD_CharacterMovementComponent::SetMovementDirectionType(EMovementDirectionType InMovementDirectionType)
{
	MovementDirectionType = InMovementDirectionType;

	HandleMovementDirection();
}



