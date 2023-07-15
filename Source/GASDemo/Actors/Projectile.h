// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameTypes.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"



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

	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);
	
};
