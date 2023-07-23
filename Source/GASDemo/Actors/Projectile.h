// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameTypes.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"


class USphereComponent;
class UParticleSystemComponent;
class UCameraShakeBase;

UCLASS()
class GASDEMO_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	const UProjectileStaticData* GetProjectileStaticData() const;

	UPROPERTY(BlueprintReadOnly , Replicated)
	TSubclassOf<UProjectileStaticData> ProjectileDataClass;

	void SetProjectileEffect(UParticleSystem* InParticleEffect , TSubclassOf<UCameraShakeBase> InImpactShake);

	void SetVelocity(FVector Velocity);

	void OnReturnToPool();

	UFUNCTION(NetMulticast , Unreliable)
	void SetActivate(FTransform Transform);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Overridable function called whenever this actor is being removed from a level */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	class UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	void DebugDrawPath() const;

	UPROPERTY()
	UStaticMeshComponent* StaticMeshComponent = nullptr;

	UPROPERTY()
	USphereComponent* SphereComponent;
	
	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);

	UFUNCTION()
	void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* EffectComp;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	TSubclassOf<UCameraShakeBase> ImpactShake;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	float ImpactShakeInnerRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	float ImpactShakeOuterRadius;
};
