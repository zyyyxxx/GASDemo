// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryList.h"
#include "GameTypes.h"
#include "IDetailTreeNode.h"
#include "InventoryItemInstance.h"
#include "DSP/AudioDebuggingUtilities.h"

void FInventoryList::AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	FInventoryListItem& Item = Items.AddDefaulted_GetRef();
	Item.ItemInstance = NewObject<UInventoryItemInstance>();
	Item.ItemInstance->Init(InItemStaticDataClass);
	MarkItemDirty(Item);
	
}

void FInventoryList::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	for(auto ItemIter  = Items.CreateIterator(); ItemIter ; ++ItemIter )
	{
		FInventoryListItem& Item = *ItemIter;
		if(Item.ItemInstance && Item.ItemInstance->GetItemStaticData()->IsA(InItemStaticDataClass))
		{
			ItemIter.RemoveCurrent();
			MarkArrayDirty(); /** This must be called if you just remove something from the array */
			break;
		}
	}
}
