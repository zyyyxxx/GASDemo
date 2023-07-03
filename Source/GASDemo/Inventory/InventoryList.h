// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryList.generated.h"

class UInventoryItemInstance;
class UItemStaticData;

USTRUCT(BlueprintType)
struct FInventoryListItem : public FFastArraySerializerItem /** Base struct for items using Fast TArray Replication */
{
	GENERATED_BODY()

public:

	UPROPERTY()
	UInventoryItemInstance* ItemInstance = nullptr;
	
};

USTRUCT(BlueprintType)
struct FInventoryList : public FFastArraySerializer ///** Base struct for wrapping the array used in Fast TArray Replication */
{
	GENERATED_BODY()

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryListItem , FInventoryList>(Items , DeltaParams , *this);
	}

	void AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);
	void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

	TArray<FInventoryListItem>& GetItemsRef(){return Items;}
	
protected:

	UPROPERTY()
	TArray<FInventoryListItem> Items;
};

template<>
struct TStructOpsTypeTraits<FInventoryList>: public TStructOpsTypeTraitsBase2<FInventoryList> /** type traits to cover the custom aspects of a script struct **/
{
	enum{WithNetDeltaSerializer = true}; // struct has a NetDeltaSerialize function for serializing differences in state from a previous NetSerialize operation.
};