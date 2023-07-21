// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/ProjectilePool.h"

#include "Actors/Projectile.h"
#include "GameTypes.h"


UProjectilePool::UProjectilePool()
{
}

AProjectile* UProjectilePool::GetProjectileFromPool(TSubclassOf<UProjectileStaticData> ProjectileStaticData)
{
	// 检查对象池中是否有池子
	if (ProjectilePool.Contains(ProjectileStaticData.GetDefaultObject()->ProjectileType))
	{
		const UProjectileStaticData* ProjectileStatic = GetDefault<UProjectileStaticData>(ProjectileStaticData);
		TArray<AProjectile*>& ProjectileArray = ProjectilePool.FindOrAdd(ProjectileStatic->ProjectileType);
		if (ProjectileArray.Num() > 0)
		{
			// 从对象池中取出子弹
			AProjectile* Projectile = ProjectileArray.Pop();
			
			if(bDebug)
			{
				ProjectileArray = ProjectilePool.FindOrAdd(ProjectileStaticData.GetDefaultObject()->ProjectileType);
				int num = ProjectileArray.Num();
		
				FString BulletTypeName;
				switch (ProjectileStaticData.GetDefaultObject()->ProjectileType)
				{
				case EProjectileType::Pistol:
					BulletTypeName = FString(TEXT("Pistol"));
					break;
				case EProjectileType::Rifle:
					BulletTypeName = FString(TEXT("Rifle"));
					break;
				case EProjectileType::Rocket:
					BulletTypeName = FString(TEXT("Rocket"));
					break;
				default:
					BulletTypeName = FString(TEXT("Invalid"));
					break;
				}
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("Get--Bullet Type: %s, Count: %d"), *BulletTypeName, num));
			}
			
			return Projectile;
		}
		else
		{
			// 如果对象池中没有对应类型的子弹，创建一个新的子弹并返回
			APawn* Owner = Cast<APawn>(GetOuter());
			FTransform Transform = Owner->GetTransform();
			AProjectile* NewProjectile = Owner->GetWorld()->SpawnActorDeferred<AProjectile>(AProjectile::StaticClass(),
						Transform , Owner , Owner , ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			NewProjectile->ProjectileDataClass = ProjectileStaticData;
			NewProjectile->FinishSpawning(Transform);
			HideProjectile(NewProjectile);
			
			return NewProjectile;
		}
	}
	
	return nullptr;
}

void UProjectilePool::ReturnProjectileToPool(AProjectile* Projectile)
{
	// Hide the bullet and return it to the pool
	HideProjectile(Projectile);
	Projectile->SetVelocity(FVector::ZeroVector);
	
	// Return the bullet to the pool
	const UProjectileStaticData* ProjectileStaticData = GetDefault<UProjectileStaticData>(Projectile->ProjectileDataClass);
	const EProjectileType ProjectileType = ProjectileStaticData->ProjectileType;
	TArray<AProjectile*>& ProjectileArray = ProjectilePool.FindOrAdd(ProjectileType);
	
	ProjectileArray.AddUnique(Projectile);

	

	if(bDebug)
	{
		ProjectileArray = ProjectilePool.FindOrAdd(ProjectileType);
		int num = ProjectileArray.Num();
		
		FString BulletTypeName;
		switch (ProjectileType)
		{
		case EProjectileType::Pistol:
			BulletTypeName = FString(TEXT("Pistol"));
			break;
		case EProjectileType::Rifle:
			BulletTypeName = FString(TEXT("Rifle"));
			break;
		case EProjectileType::Rocket:
			BulletTypeName = FString(TEXT("Rocket"));
			break;
		default:
			BulletTypeName = FString(TEXT("Invalid"));
			break;
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("Return--Bullet Type: %s, Count: %d"), *BulletTypeName, num));
	}
}



void UProjectilePool::InitProjectilePool()
{
	FirstInit = false;
	constexpr int32 PoolSize = 10; 
	// 添加不同类型的子弹到对象池中
	for (auto ProjectileStaticData : DefaultProjectileStaticDatas)
	{
		EProjectileType ProjectileType = ProjectileStaticData.GetDefaultObject()->ProjectileType;
		TArray<AProjectile*>& ProjectileArray = ProjectilePool.FindOrAdd(ProjectileType);

		// 预先创建一些子弹对象并添加到对象池中
		for (int32 i = 0; i < PoolSize; ++i)
		{
			APawn* Owner = Cast<APawn>(GetOuter());
			FTransform Transform = Owner->GetTransform();
			AProjectile* NewProjectile = Owner->GetWorld()->SpawnActorDeferred<AProjectile>(AProjectile::StaticClass(),
				Transform , Owner , Owner , ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			HideProjectile(NewProjectile);
			NewProjectile->ProjectileDataClass = ProjectileStaticData;
			NewProjectile->FinishSpawning(Transform);
			ProjectileArray.Add(NewProjectile);
			
		}
	}
}

void UProjectilePool::HideProjectile(AProjectile* Projectile)
{
	// Hide the bullet
	Projectile->SetActorLocation(FVector(0.f, 0.f, -10000.f)); // Move the bullet to a hidden location
}




