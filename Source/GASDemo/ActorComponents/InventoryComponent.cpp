// Fill out your copyright notice in the Description page of Project Settings.



#include "ActorComponents/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

#include "Inventory/InventoryList.h"
#include "Inventory/InventoryItemInstance.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Actors/ItemActor.h"

#include "GameplayTagsManager.h"

#include "Engine/ActorChannel.h"

FGameplayTag UInventoryComponent::EquipItemActorTag;
FGameplayTag UInventoryComponent::DropItemTag;
FGameplayTag UInventoryComponent::EquipNextTag;
FGameplayTag UInventoryComponent::UnequipTag;

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

	static bool bHandledAddingTags = false;
	if(!bHandledAddingTags)
	{
		bHandledAddingTags = true;
		UGameplayTagsManager::Get().OnLastChanceToAddNativeTags().AddUObject(this , &UInventoryComponent::AddInventoryTags);
	}
	
}

void UInventoryComponent::AddInventoryTags()
{
	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();

	UInventoryComponent::EquipItemActorTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.EquipItemActor"),TEXT("Equip item from item actor event"));
	UInventoryComponent::DropItemTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.DropItemTag"),TEXT("Drop equipped item"));
	UInventoryComponent::EquipNextTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.EquipNextTag"),TEXT("Try equip next item"));
	UInventoryComponent::UnequipTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.UnequipTag"),TEXT("Unequip current item"));
	
	TagsManager.OnLastChanceToAddNativeTags().RemoveAll(this);
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

	// 使用ASC的gameplayevent来调用
	if(UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())){
		ASC->GenericGameplayEventCallbacks.FindOrAdd(EquipItemActorTag).AddUObject(this , &UInventoryComponent::GameplayEventCallBack);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(DropItemTag).AddUObject(this , &UInventoryComponent::GameplayEventCallBack);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(EquipNextTag).AddUObject(this , &UInventoryComponent::GameplayEventCallBack);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(UnequipTag).AddUObject(this , &UInventoryComponent::GameplayEventCallBack);
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
	if(GetOwner()->HasAuthority())
	{
		InventoryList.AddItem(InItemStaticDataClass);
	}
}

void UInventoryComponent::AddItemInstance(UInventoryItemInstance* InItemInstance)
{
	if(GetOwner()->HasAuthority())
	{
		InventoryList.AddItem(InItemInstance);
	}
}

void UInventoryComponent::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	if(GetOwner()->HasAuthority())
	{
		InventoryList.RemoveItem(InItemStaticDataClass);
	}
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

void UInventoryComponent::EquipItemInstance(UInventoryItemInstance* InItemInstance)
{
	if(GetOwner()->HasAuthority())
	{
		// 装备list中的item
		for(auto Item : InventoryList.GetItemsRef())
		{
			if(Item.ItemInstance == InItemInstance)
			{
				Item.ItemInstance->OnEquipped(GetOwner());
				CurrentItem = Item.ItemInstance;
				break;
			}
		}
	}
}

void UInventoryComponent::UnequipItem()
{
	if(GetOwner()->HasAuthority())
	{
		if(IsValid(CurrentItem))
		{
			CurrentItem->OnUnEquipped(GetOwner());
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
			CurrentItem->OnDropped(GetOwner());
			RemoveItem(CurrentItem->ItemStaticDataClass);
			CurrentItem = nullptr;
		}
	}
}

UInventoryItemInstance* UInventoryComponent::GetEquippedItem() const
{
	return CurrentItem;
}

void UInventoryComponent::EquipNext()
{
	TArray<FInventoryListItem>& Items  = InventoryList.GetItemsRef();

	const bool bNoItems = Items.Num() == 0;
	const bool bOneAndEquipped = Items.Num() == 1 && CurrentItem;

	if(bNoItems || bOneAndEquipped) return;

	UInventoryItemInstance* TargetItem = CurrentItem;

	// 寻找第一个可装备的item
	for(auto Item : Items)
	{
		if(Item.ItemInstance->GetItemStaticData()->bCanEquipped)
		{
			if(Item.ItemInstance != CurrentItem)
			{
				TargetItem = Item.ItemInstance;
				break;
			}
		}
	}

	//尝试装备
	if(CurrentItem)
	{
		if(TargetItem == CurrentItem)
		{
			return;
		}
		// 可装备的第一个Item 不是当前装备的 先unequip
		UnequipItem();
		
	}
	
	EquipItemInstance(TargetItem);
}

void UInventoryComponent::GameplayEventCallBack(const FGameplayEventData* Payload)
{
	ENetRole NetRole = GetOwnerRole();
	if(NetRole == ROLE_Authority)
	{
		HandleGameplayEventInternal(*Payload);
	}else if(NetRole == ROLE_AutonomousProxy)
	{
		ServerHandleGameplayEvent(*Payload);
	}
}


void UInventoryComponent::HandleGameplayEventInternal(FGameplayEventData Payload)
{
	ENetRole NetRole = GetOwnerRole();
	if(NetRole == ROLE_Authority)
	{
		FGameplayTag EventTag = Payload.EventTag;

		if(EventTag == EquipItemActorTag)
		{
			if(const UInventoryItemInstance* ItemInstance = Cast<UInventoryItemInstance>(Payload.OptionalObject))
			{
				AddItemInstance(const_cast<UInventoryItemInstance*>(ItemInstance));

				if(Payload.Instigator)
				{
					const_cast<AActor*>(Payload.Instigator.Get())->Destroy();
				}
			}
		}
		else if(EventTag == EquipNextTag)
		{
			EquipNext();
		}
		else if(EventTag == DropItemTag)
		{
			DropItem();
		}
		else if(EventTag == UnequipTag)
		{
			UnequipItem();
		}
	}
}

void UInventoryComponent::ServerHandleGameplayEvent_Implementation(FGameplayEventData Payload)
{
	HandleGameplayEventInternal(Payload);
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
