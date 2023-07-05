// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameTypes.h"
#include "InventoryItemInstance.generated.h"

/**
 * 
 */
UCLASS()
class GASDEMO_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()
public:
	virtual void Init(TSubclassOf<UItemStaticData> InItemStaticDataClass);

	// 令UObject支持网络复制
	virtual bool IsSupportedForNetworking() const override{return true;}

	UFUNCTION(BlueprintCallable , BlueprintPure)
	const UItemStaticData* GetItemStaticData() const;

	UPROPERTY(Replicated)
	TSubclassOf<UItemStaticData> ItemStaticDataClass;

	UPROPERTY(ReplicatedUsing = OnRep_Equipped)
	bool bEquipped = false;

	UFUNCTION()
	void OnRep_Equipped();

	virtual void OnEquipped(AActor* InOwner = nullptr);
	virtual void OnUnEquipped();
	virtual void OnDropped();

protected:

	UPROPERTY(Replicated)
	AItemActor* ItemActor = nullptr;
	
};
