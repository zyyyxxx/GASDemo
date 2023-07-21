// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GDGameStatics.generated.h"

class UProjectilePool;
class UItemStaticData;

UCLASS()
class GASDEMO_API UGDGameStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable , BlueprintPure)
	static const UItemStaticData* GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass);

	// 施加伤害与GE
	UFUNCTION(BlueprintCallable , meta = (WorldContext = "WorldContextObject"))
	static void ApplyRadialDamage(UObject* WorldContextObject , AActor* DamageCauser ,
		FVector Location , float Radius ,
		float DamageAmount , TArray<TSubclassOf<class UGameplayEffect>> DamageEffects ,
		const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes , ETraceTypeQuery TraceType);

	// 发射子弹
	UFUNCTION(BlueprintCallable , meta = (WorldContext = "WorldContextObject"))
	static AProjectile* LaunchProjectile(UObject* WorldContextObject , TSubclassOf<UProjectileStaticData> ProjectileDataClass,
	FTransform Transform , AActor* Owner , APawn* Instigator , UParticleSystem* ParticleEffect = nullptr, TSubclassOf<UCameraShakeBase> ImpactShake = nullptr , UProjectilePool* ProjectilePool = nullptr);
};
