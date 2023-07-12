// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "GD_PhysicalMaterial.generated.h"

class USoundBase;
class UNiagaraSystem;

UCLASS()
class GASDEMO_API UGD_PhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = PhysicalMatrial)
	USoundBase* FootStepSound = nullptr;

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = PhysicalMatrial)
	USoundBase* PointImpactSound = nullptr;

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = PhysicalMatrial)
	UNiagaraSystem* PointImpactVFX = nullptr;
	
};
