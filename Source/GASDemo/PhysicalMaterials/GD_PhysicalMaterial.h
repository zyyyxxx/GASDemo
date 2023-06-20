// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "GD_PhysicalMaterial.generated.h"

/**
 * 
 */
UCLASS()
class GASDEMO_API UGD_PhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = PhysicalMatrial)
	class USoundBase* FootStepSound = nullptr;
};
