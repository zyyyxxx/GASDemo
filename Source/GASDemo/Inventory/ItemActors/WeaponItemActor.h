// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ItemActor.h"
#include "WeaponItemActor.generated.h"


UCLASS()
class GASDEMO_API AWeaponItemActor : public AItemActor
{
	GENERATED_BODY()
public:
	AWeaponItemActor();

	const UWeaponStaticData* GetWeaponStaticData() const;

	UFUNCTION(BlueprintPure)
	FVector GetMuzzleLocation() const;

	UFUNCTION(BlueprintCallable)
	void PlayweaponEffects(const FHitResult& InHitResult);


protected:
	UFUNCTION(Server , Reliable)
	void ServerPlayweaponEffects(const FHitResult& InHitResult);
	
	UFUNCTION(NetMulticast , Reliable)
	void MulticastPlayweaponEffects(const FHitResult& InHitResult);

	void PlayweaponEffectsInternal(const FHitResult& InHitResult);

	UPROPERTY()
	UMeshComponent* MeshComponent = nullptr;

	virtual void InitInternal() override;
	
};
