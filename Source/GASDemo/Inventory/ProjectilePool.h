// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProjectilePool.generated.h"

class UProjectileStaticData;
enum class EProjectileType : uint8;
class AProjectile;
/**
 * 
 */
UCLASS()
class GASDEMO_API UProjectilePool : public UActorComponent
{
	GENERATED_BODY()

public:
	bool FirstInit = true;

	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	bool bDebug = true;
	
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly)
	TArray<TSubclassOf<UProjectileStaticData>> DefaultProjectileStaticDatas;
	
	// Constructor
	UProjectilePool();

	// Function to get a bullet from the pool
	AProjectile* GetProjectileFromPool(TSubclassOf<UProjectileStaticData> ProjectileStaticData);

	// Function to return a bullet to the pool
	void ReturnProjectileToPool(AProjectile* Projectile);

	void HideProjectile(AProjectile* Projectile);

	void InitProjectilePool();

private:
	TMap<EProjectileType , TArray<AProjectile*>> ProjectilePool;
};
