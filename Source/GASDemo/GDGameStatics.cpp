// Fill out your copyright notice in the Description page of Project Settings.


#include "GDGameStatics.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameTypes.h"
#include "Actors/Projectile.h"
#include "Inventory/ProjectilePool.h"
#include "Kismet/KismetSystemLibrary.h"

static TAutoConsoleVariable<int32> CVarShowRadialDamage
{
	TEXT("ShowRadialDamage"),
	0,
	TEXT("Draws Debug info about radial damage")
	TEXT("0: off/n")
	TEXT("1: on/n"),
	ECVF_Cheat
};

const UItemStaticData* UGDGameStatics::GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass)
{
	if(IsValid(ItemDataClass))
	{
		return GetDefault<UItemStaticData>(ItemDataClass); // 返回类的默认对象
	}

	return nullptr;
}

void UGDGameStatics::ApplyRadialDamage(UObject* WorldContextObject, AActor* DamageCauser, FVector Location, float Radius,
	float DamageAmount, TArray<TSubclassOf<UGameplayEffect>> DamageEffects,
	const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, ETraceTypeQuery TraceType)
{
	TArray<AActor*> OutActors;
	TArray<AActor*> ActorToIgnore = {DamageCauser};

	UKismetSystemLibrary::SphereOverlapActors(WorldContextObject , Location , Radius , ObjectTypes ,
		nullptr , ActorToIgnore , OutActors);

	const bool bDebug = static_cast<bool>(CVarShowRadialDamage.GetValueOnAnyThread());
	// 遍历所有在攻击范围内的对象
	for(AActor* Actor : OutActors)
	{
		FHitResult HitResult;
		// 进行射线检测 检测是否被遮挡 
		if(UKismetSystemLibrary::LineTraceSingle(WorldContextObject , Location , Actor->GetActorLocation() , TraceType ,
		true , ActorToIgnore , EDrawDebugTrace::None , HitResult , true))
		{
			AActor* Target = HitResult.GetActor();
			
			if(Target == Actor)
			{
				bool bWasApplied = false;
				
				if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
				{
					FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
					EffectContext.AddInstigator(DamageCauser , DamageCauser);

					for(auto Effect : DamageEffects)
					{
						FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect , 1, EffectContext);
						if(SpecHandle.IsValid())
						{
							// 修改生命GE
							UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle ,
								FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")) , -DamageAmount);

							FActiveGameplayEffectHandle ActiveGEHandle =
								AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

							if(ActiveGEHandle.WasSuccessfullyApplied())
							{
								bWasApplied = true;
							}
						}
					}
				}

				if(bDebug)
				{
					DrawDebugLine(WorldContextObject->GetWorld() , Location , Actor->GetActorLocation() ,
						bWasApplied ? FColor::Green : FColor::Red , false , 4.f , 0 , 1);
					DrawDebugSphere(WorldContextObject->GetWorld() , Location , 16, 16 ,
						bWasApplied ? FColor::Green : FColor::Red , false , 4.f , 0 , 1);

					DrawDebugString(WorldContextObject->GetWorld() , HitResult.Location , *GetNameSafe(Target) , nullptr ,
						FColor::White, 0 , false , 1.f);
				}
			}
			else
			{
				if(bDebug)
				{
					DrawDebugLine(WorldContextObject->GetWorld() , Location , Actor->GetActorLocation() ,
						FColor::Red , false , 4.f , 0 , 1);
					DrawDebugSphere(WorldContextObject->GetWorld() , Location , 16, 16 ,
						FColor::Red , false , 4.f , 0 , 1);

					DrawDebugString(WorldContextObject->GetWorld() , HitResult.Location , *GetNameSafe(Target) , nullptr ,
						FColor::Red, 0 , false , 1.f);
				}
			}
		}
		else
		{
			if(bDebug)
			{
				DrawDebugLine(WorldContextObject->GetWorld() , Location , Actor->GetActorLocation() ,
					FColor::Red , false , 4.f , 0 , 1);
				DrawDebugSphere(WorldContextObject->GetWorld() , Location , 16, 16 ,
					FColor::Red , false , 4.f , 0 , 1);

				DrawDebugString(WorldContextObject->GetWorld() , HitResult.Location , *GetNameSafe(HitResult.GetActor()) , nullptr ,
					FColor::Red, 0 , false , 1.f);
			}
		}
	}

	if(bDebug)
	{
		DrawDebugSphere(WorldContextObject->GetWorld() , Location , Radius, 16 ,
					FColor::White , false , 4.f , 0 , 1);
	}
}



AProjectile* UGDGameStatics::LaunchProjectile(UObject* WorldContextObject,
                                              TSubclassOf<UProjectileStaticData> ProjectileDataClass, FTransform Transform, AActor* Owner, APawn* Instigator ,
                                              UParticleSystem* ParticleEffect, TSubclassOf<UCameraShakeBase> ImpactShake , UProjectilePool* ProjectilePool)
{
	UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;

	if(World && Owner->HasAuthority()) // 服务器生成子弹
	{
		AProjectile* Projectile = nullptr;
		if(IsValid(ProjectilePool))
		{
			if(ProjectilePool->FirstInit)
			{
				ProjectilePool->InitProjectilePool();
			}
			
			Projectile = ProjectilePool->GetProjectileFromPool(ProjectileDataClass);

			Projectile->SetActivate(Transform);
			

			
			if(ParticleEffect && ImpactShake)
			{
				Projectile->SetProjectileEffect(ParticleEffect , ImpactShake);
				APawn * MyOwner = Cast<APawn>(Instigator);
				if (MyOwner)
				{
					APlayerController * PC = Cast<APlayerController>(MyOwner->GetController());
					if (PC)
					{
						APlayerCameraManager* CameraManager = PC->PlayerCameraManager;
						if(CameraManager)
						{
							CameraManager->StartCameraShake(ImpactShake , 2);
						}
					}
				}
			}
		}

		if(Projectile) return Projectile;
		
		if(!Projectile)
		{
			Projectile = World->SpawnActorDeferred<AProjectile>(AProjectile::StaticClass() ,Transform , Owner , Instigator , ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			if(ParticleEffect && ImpactShake)
			{
				Projectile->SetProjectileEffect(ParticleEffect , ImpactShake);
				APawn * MyOwner = Cast<APawn>(Instigator);
				if (MyOwner)
				{
					APlayerController * PC = Cast<APlayerController>(MyOwner->GetController());
					if (PC)
					{
						APlayerCameraManager* CameraManager = PC->PlayerCameraManager;
						if(CameraManager)
						{
							CameraManager->StartCameraShake(ImpactShake , 2);
						}
					}
				}
			}
			Projectile->ProjectileDataClass = ProjectileDataClass;
			Projectile->FinishSpawning(Transform);

			return Projectile;
		}

		

		
	}

	return nullptr;
}
