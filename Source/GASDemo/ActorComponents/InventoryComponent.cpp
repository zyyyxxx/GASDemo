// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/InventoryComponent.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/InventoryList.h"
#include  "Inventory/InventoryItemInstance.h"

static TAutoConsoleVariable<int32> CVarShowInventory(
	TEXT("ShowDebugInventory"),
	0,
	TEXT("Draw debug info about Inventory")
	TEXT("0: off/n")
	TEXT("1: on/n"),
	ECVF_Cheat
);


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);

}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Server only
	if(GetOwner()->HasAuthority())
	{
		for(auto ItemClass : DefaultItems)
		{
			InventoryList.AddItem(ItemClass);
		}
		
	}
	
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for(FInventoryListItem& Item : InventoryList.GetItemsRef())
	{
		UInventoryItemInstance* ItemInstance = Item.ItemInstance;

		if(IsValid(ItemInstance))
		{
			WroteSomething |= Channel->ReplicateSubobject(ItemInstance , *Bunch , *RepFlags); // Replicates given subobject on this actor channel
		}
		
	}
	return WroteSomething;
}

void UInventoryComponent::AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	InventoryList.AddItem(InItemStaticDataClass);
}

void UInventoryComponent::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	InventoryList.RemoveItem(InItemStaticDataClass);
}

void UInventoryComponent::EquipItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	if(GetOwner()->HasAuthority())
	{
		for(auto Item : InventoryList.GetItemsRef())
		{
			if(Item.ItemInstance->ItemStaticDataClass == InItemStaticDataClass)
			{
				Item.ItemInstance->OnEquipped(GetOwner());
				CurrentItem = Item.ItemInstance;
				break;	
			}
		}
	}
}

void UInventoryComponent::UnEquipItem()
{
	if(GetOwner()->HasAuthority())
	{
		if(IsValid(CurrentItem))
		{
			CurrentItem->OnUnEquipped();
			CurrentItem = nullptr;
		}
	}
}

void UInventoryComponent::DropItem()
{
	if(GetOwner()->HasAuthority())
	{
		if(IsValid(CurrentItem))
		{
			CurrentItem->OnDropped();
			RemoveItem(CurrentItem->ItemStaticDataClass);
			CurrentItem = nullptr;
		}
	}
}

UInventoryItemInstance* UInventoryComponent::GetEquippedItem() const
{
	return CurrentItem;
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Debug
	const bool bShowDebug = CVarShowInventory.GetValueOnGameThread() != 0;
	if(bShowDebug)
	{
		for(FInventoryListItem& Item : InventoryList.GetItemsRef())
		{
			UInventoryItemInstance* ItemInstance = Item.ItemInstance;
			const UItemStaticData* ItemStaticData = ItemInstance->GetItemStaticData();
			
			if(IsValid(ItemInstance) && IsValid(ItemStaticData))
			{
				GEngine->AddOnScreenDebugMessage(-1 , 0 ,FColor::Blue , FString::Printf(TEXT("Item : %s") , *ItemStaticData->Name.ToString()));
			}
		}
	}

}


void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent , InventoryList);
	DOREPLIFETIME(UInventoryComponent , CurrentItem);
}
