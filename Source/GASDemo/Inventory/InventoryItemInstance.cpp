// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItemInstance.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GDGameStatics.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Actors/ItemActor.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

void UInventoryItemInstance::Init(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	ItemStaticDataClass = InItemStaticDataClass;
}

const UItemStaticData* UInventoryItemInstance::GetItemStaticData() const
{
	return UGDGameStatics::GetItemStaticData(ItemStaticDataClass);
}

void UInventoryItemInstance::OnRep_Equipped()
{
}

void UInventoryItemInstance::OnEquipped(AActor* InOwner)
{
	if(UWorld* World = InOwner->GetWorld())
	{

		const UItemStaticData* StaticData = GetItemStaticData();
		
		FTransform Transform;
		ItemActor = World->SpawnActorDeferred<AItemActor>(GetItemStaticData()->ItemActorClass , Transform , InOwner);
		ItemActor->Init(this);
		ItemActor->OnEquipped();
		ItemActor->FinishSpawning(Transform);

		ACharacter* Character = Cast<ACharacter>(InOwner);
		if(USkeletalMeshComponent* SkeletalMesh = Character ? Character->GetMesh() : nullptr)
		{
			ItemActor->AttachToComponent(SkeletalMesh , FAttachmentTransformRules::SnapToTargetNotIncludingScale , StaticData->AttachmentSocket);
		}
	}

	TryGrantAbilities(InOwner);
	
	bEquipped = true;
}

void UInventoryItemInstance::OnUnEquipped(AActor* InOwner)
{
	if(ItemActor)
	{
		ItemActor->Destroy();
		ItemActor = nullptr;
	}

	TryRemoveAbilities(InOwner);
	
	bEquipped = false;
}

void UInventoryItemInstance::OnDropped(AActor* InOwner)
{
	if(ItemActor)
	{
		ItemActor->OnDropped();
	}

	TryRemoveAbilities(InOwner);
	
	bEquipped = false;
}

void UInventoryItemInstance::TryGrantAbilities(AActor* InOwner)
{
	if(InOwner && InOwner->HasAuthority())
	{
		if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
		{
			const UItemStaticData* StaticData = GetItemStaticData();

			for(auto ItemAbility : StaticData->GrantedAbilities)
			{
				GrantedAbilityHandles.Add(AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(ItemAbility)));
			}
		}
	}
}

void UInventoryItemInstance::TryRemoveAbilities(AActor* InOwner)
{
	if(InOwner && InOwner->HasAuthority())
	{
		if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
		{
			const UItemStaticData* StaticData = GetItemStaticData();

			for(auto AbilitySpecHandle : GrantedAbilityHandles)
			{
				AbilitySystemComponent->ClearAbility(AbilitySpecHandle);
			}
			GrantedAbilityHandles.Empty();
		}
	}
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItemInstance , ItemStaticDataClass);
	DOREPLIFETIME(UInventoryItemInstance , bEquipped);
	DOREPLIFETIME(UInventoryItemInstance , ItemActor);
}
