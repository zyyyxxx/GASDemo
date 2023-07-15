// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Projectile.h"

#include "GDGameStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/GD_CharacterBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<int32> CVarShowProjetiles
{
	TEXT("ShowDebugProjectiles"),
	0,
	TEXT("Draws Debug info about projectiles")
	TEXT("0: off/n")
	TEXT("1: on/n"),
	ECVF_Cheat
};

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    AActor::SetReplicateMovement(true);
	bReplicates = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->Velocity = FVector::ZeroVector;
	ProjectileMovementComponent->OnProjectileStop.AddDynamic(this , &AProjectile::OnProjectileStop);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	StaticMeshComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetIsReplicated(true);
	StaticMeshComponent->SetCollisionProfileName(TEXT("Projectile"));
	StaticMeshComponent->bReceivesDecals = false; //不接受Decal Decal经常被用在 显示弹痕，地面叠加花纹等
	StaticMeshComponent->SetRelativeScale3D(FVector(0.2,0.2,0.2));
}

const UProjectileStaticData* AProjectile::GetProjectileStaticData() const
{
	if(IsValid(ProjectileDataClass))
	{
		return GetDefault<UProjectileStaticData>(ProjectileDataClass);
	}
	return nullptr;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();

	// 设置外观与ProjectileMovementComponent基础配置
	if(ProjectileStaticData && ProjectileMovementComponent)
	{
		if(ProjectileStaticData->StaticMesh)
		{
			StaticMeshComponent->SetStaticMesh(ProjectileStaticData->StaticMesh);
		}

		ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;
		ProjectileMovementComponent->InitialSpeed = ProjectileStaticData->InitialSpeed;
		ProjectileMovementComponent->MaxSpeed = ProjectileStaticData->MaxSpeed;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->Bounciness = 0.f;
		ProjectileMovementComponent->ProjectileGravityScale = ProjectileStaticData->GravityMultiplayer;

		ProjectileMovementComponent->Velocity = ProjectileStaticData->InitialSpeed * GetActorForwardVector();

		
	}

	const int32 DebugShowProjectile = CVarShowProjetiles.GetValueOnAnyThread();

	if(DebugShowProjectile)
	{
		DebugDrawPath();
	}
	
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();
	// 播放效果
	if(ProjectileStaticData)
	{
		UGameplayStatics::PlaySoundAtLocation(this , ProjectileStaticData->OnStopSFX , GetActorLocation() , 1.f);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this , ProjectileStaticData->OnStopVFX , GetActorLocation());
	}
	Super::EndPlay(EndPlayReason);
}

void AProjectile::DebugDrawPath() const
{
	const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();

	if(ProjectileStaticData)
	{
		FPredictProjectilePathParams PredictProjectilePathParams;
		PredictProjectilePathParams.StartLocation = GetActorLocation();
		PredictProjectilePathParams.LaunchVelocity = ProjectileStaticData->InitialSpeed * GetActorForwardVector();
		PredictProjectilePathParams.TraceChannel = ECollisionChannel::ECC_Visibility;
		PredictProjectilePathParams.bTraceComplex = true;// Trace against complex collision (triangles rather than simple primitives) if tracing with collision.
		PredictProjectilePathParams.bTraceWithCollision = true;
		PredictProjectilePathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
		PredictProjectilePathParams.DrawDebugTime = 3.f;
		PredictProjectilePathParams.OverrideGravityZ = ProjectileStaticData->GravityMultiplayer == 0.f ?
			0.0001f : ProjectileStaticData->GravityMultiplayer;
		// Optional override of Gravity (if 0, uses WorldGravityZ).

		FPredictProjectilePathResult PredictProjectilePathResult;
		if(UGameplayStatics::PredictProjectilePath(this , PredictProjectilePathParams , PredictProjectilePathResult))
		{
			DrawDebugSphere(GetWorld() , PredictProjectilePathResult.HitResult.Location , 50 ,10 , FColor::Red);
			
		}
		
	}
}

void AProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();

	if(ProjectileStaticData)
	{
		UGDGameStatics::ApplyRadialDamage(this , GetOwner() , GetActorLocation() ,
			ProjectileStaticData->DamageRadius,
			ProjectileStaticData->BaseDamage,
			ProjectileStaticData->Effects ,
			ProjectileStaticData->RadialDamageQueryTypes,
			ProjectileStaticData->RadialDamageTraceType);
	}
	// Projectile停下后销毁 触发EndPlay
	Destroy();
}


void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectile , ProjectileDataClass);
}

