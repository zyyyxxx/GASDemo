// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GD_CharacterBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemLog.h"
#include "ShaderPrintParameters.h"
#include "ActorComponents/FootstepsComponent.h"
#include "DataAssets/CharacterDataAsset.h"
#include "GASDemo/AbilitySystem/Attributes/GD_AttributeSet.h"
#include "GASDemo/AbilitySystem/Components/GD_AbilitySystemComponent.h"

#include "Net/UnrealNetwork.h"
#include "ActorComponents/GD_CharacterMovementComponent.h"
#include "ActorComponents/GD_MotionWarpingComponent.h"


AGD_CharacterBase::AGD_CharacterBase(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer.SetDefaultSubobjectClass<UGD_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName)) // 修改父类组件创建类型
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 250.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	GDCharacterMovementComponent = Cast<UGD_CharacterMovementComponent>(GetCharacterMovement());

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Ability System
	AbilitySystemComponent = CreateDefaultSubobject<UGD_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UGD_AttributeSet>(TEXT("AttributeSet"));

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxMovementSpeedAttribute()).AddUObject(this, &AGD_CharacterBase::OnMaxMovementChanged); // 注册函数到此速度attribute改变多播委托

	FootstepsComponent = CreateDefaultSubobject<UFootstepsComponent>(TEXT("FootstepsComponent"));

	GDMotionWarpingComponent = CreateDefaultSubobject<UGD_MotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	
}

void AGD_CharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(IsValid(CharacterDataAsset))
	{
		SetCharacterData(CharacterDataAsset->CharacterData);
	}
}

UAbilitySystemComponent* AGD_CharacterBase::GetAbilitySystemComponent () const
{
	return AbilitySystemComponent;
}

bool AGD_CharacterBase::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect,
                                                  FGameplayEffectContextHandle InEffectContext)
{
	if(!Effect.Get()) return false; // 检查是否valid

	// MakeOutgoingSpec : Get an outgoing GameplayEffectSpec that is ready to be applied to other things
	
	/*FGameplayEffectSpecHandle是GAS（Gameplay Ability System）中的一个句柄（Handle），用于表示游戏效果规范（Gameplay Effect Spec）的引用。
	MakeOutgoingSpec方法创建一个用于应用游戏效果的规范（Spec）。
	Effect是要应用的游戏效果的类（Class），1表示效果的等级（Level），InEffectContext是游戏效果的上下文（Context）。*/
	
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect , 1 , InEffectContext);
	
	/*FActiveGameplayEffectHandle是GAS（Gameplay Ability System）中的另一个句柄（Handle），用于表示活动游戏效果（Active Gameplay Effect）的引用。
	活动游戏效果是指已经应用于实体（例如角色）的游戏效果，即效果正在生效的状态。
	每当应用游戏效果时，都会生成一个活动游戏效果，并返回一个对其的句柄，以便稍后对其进行管理和操作。
	与FGameplayEffectSpecHandle不同，FActiveGameplayEffectHandle表示实际生效的游戏效果，而不仅仅是游戏效果规范。
	您可以使用句柄来检查、更新或移除活动游戏效果，例如修改效果参数、查询其状态、延长或缩短持续时间等。*/

	if(SpecHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		return ActiveGEHandle.WasSuccessfullyApplied();
	}
	
	return false;
}

void AGD_CharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if(AbilitySystemComponent)
	{
		// 利用Tag移除GE
		AbilitySystemComponent->RemoveActiveEffectsWithTags(InAirTags);
	}
}

void AGD_CharacterBase::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	
	if(!CrouchStateEffect.Get()) return;

	if(AbilitySystemComponent)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CrouchStateEffect , 1, EffectContext);
		if(SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if(!ActiveGEHandle.WasSuccessfullyApplied())
			{
				ABILITY_LOG(Log , TEXT("Ability %s failed to apply startup effect %s! "), *GetName(),*GetNameSafe(CrouchStateEffect));
			}
		}
	}
}

void AGD_CharacterBase::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if(AbilitySystemComponent && CrouchStateEffect.Get())
	{
		//Remove active gameplay effects whose backing definition are the specified gameplay effect class
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(CrouchStateEffect , AbilitySystemComponent);
	}
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

UGD_MotionWarpingComponent* AGD_CharacterBase::GetGDMotionWarpingComponent() const
{
	return GDMotionWarpingComponent;
}

void AGD_CharacterBase::GiveAbilities()
{
	if(HasAuthority() && AbilitySystemComponent)
	{
		for(auto DefaultAbility : CharacterData.Abilities)// 从存储的CharacterData中获取
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
	}
}

void AGD_CharacterBase::ApplyStartupEffects()
{
	if(GetLocalRole() == ROLE_Authority) // Authoritative control over the actor
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext(); // 创建EffectContext
		EffectContext.AddSourceObject(this); // Sets the object this effect was created from.

		for(auto CharacterEffect: CharacterData.Effects) // 从存储的CharacterData中获取
		{
			ApplyGameplayEffectToSelf(CharacterEffect , EffectContext);
		}
		
		
	}
}

void AGD_CharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// Server Initialization

	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	
	GiveAbilities();
	ApplyStartupEffects();
}

void AGD_CharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// Client Initialization

	AbilitySystemComponent->InitAbilityActorInfo(this,this);
}




void AGD_CharacterBase::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}



//////////////////////////////////////////////////////////////////////////
// Input

void AGD_CharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AGD_CharacterBase::OnJumpStarted);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGD_CharacterBase::OnJumpEnded);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGD_CharacterBase::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGD_CharacterBase::Look);

		//Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AGD_CharacterBase::OnCrouchStarted);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AGD_CharacterBase::OnCrouchEnded);
		
		//Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AGD_CharacterBase::OnSprintStarted);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AGD_CharacterBase::OnSprintEnded);
	}

}

void AGD_CharacterBase::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AGD_CharacterBase::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGD_CharacterBase::OnJumpStarted(const FInputActionValue& Value)
{
	/*
	FGameplayEventData PayLoad;
	PayLoad.Instigator = this;
	PayLoad.EventTag = JumpEventTag;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this , JumpEventTag ,PayLoad); // 利用tag触发BP_GA_Jump的trigger
	*/

	// 尝试Traversal 跳跃GA触发移动到GDCharacterMovementComponent
	GDCharacterMovementComponent->TryTraversal(AbilitySystemComponent);
	
}

void AGD_CharacterBase::OnJumpEnded(const FInputActionValue& Value)
{
	// 跳跃结束移除GE的功能在GD_GA基类中实现了
}

void AGD_CharacterBase::OnCrouchStarted(const FInputActionValue& Value)
{
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(CrouchTags);	
	}
}

void AGD_CharacterBase::OnCrouchEnded(const FInputActionValue& Value)
{
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities(&CrouchTags);
	}
}

void AGD_CharacterBase::OnSprintStarted(const FInputActionValue& Value)
{
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(SprintTags,true);
	}
}

void AGD_CharacterBase::OnSprintEnded(const FInputActionValue& Value)
{
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities(&SprintTags);
		
	}
}


FCharacterData AGD_CharacterBase::GetCharacterData() const
{
	return CharacterData;
}

void AGD_CharacterBase::SetCharacterData(const FCharacterData& InCharacterData)
{
	CharacterData = InCharacterData;
	InitFromCharacterData(CharacterData);
}

UFootstepsComponent* AGD_CharacterBase::GetFootstepComponent() const
{
	return FootstepsComponent;
}

void AGD_CharacterBase::OnMaxMovementChanged(const FOnAttributeChangeData& Data) 
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void AGD_CharacterBase::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication)
{
	
}

void AGD_CharacterBase::OnRep_CharacterData()
{
	InitFromCharacterData(CharacterData , true);
}

void AGD_CharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGD_CharacterBase , CharacterData);
}
