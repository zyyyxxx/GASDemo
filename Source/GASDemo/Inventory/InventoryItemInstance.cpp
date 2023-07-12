// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItemInstance.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemLog.h"
#include "GDGameStatics.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Actors/ItemActor.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

void UInventoryItemInstance::Init(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	ItemStaticDataClass = InItemStaticDataClass;
}

const UItemStaticData* UInventoryItemInstance:: GetItemStaticData() const
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

		// Spawn Actor 并 初始化
		
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

	// 添加GA与GE
	TryGrantAbilities(InOwner);
	TryApplyEffects(InOwner);
	
	bEquipped = true;
}

void UInventoryItemInstance::OnUnEquipped(AActor* InOwner)
{
	if(ItemActor)
	{
		ItemActor->Destroy();
		ItemActor = nullptr;
	}

	// 移除GA与GE
	TryRemoveAbilities(InOwner);
	TryRemoveEffects(InOwner);
	
	bEquipped = false;
}

void UInventoryItemInstance::OnDropped(AActor* InOwner)
{
	if(ItemActor)
	{
		ItemActor->OnDropped();
	}

	// 移除GA与GE
	TryRemoveAbilities(InOwner);
	TryRemoveEffects(InOwner);
	
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

void UInventoryItemInstance::TryApplyEffects(AActor* InOwner)
{
	if(UAbilitySystemComponent* AbilityComponent =  UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
	{
		const UItemStaticData* ItemStaticData = GetItemStaticData();
		const FGameplayEffectContextHandle EffectContext = AbilityComponent->MakeEffectContext();
		
		for(auto GameplayEffect : ItemStaticData->OngoingEffects)
		{
			if(!GameplayEffect.Get()) continue;

			
			FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(GameplayEffect , 1, EffectContext);
			if(SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				if(!ActiveGEHandle.WasSuccessfullyApplied())
				{
					ABILITY_LOG(Log , TEXT("Item %s failed to apply runtime effect %s! "), *GetName(),*GetNameSafe(GameplayEffect));
				}
				else
				{
					OngoingEffectHandles.Add(ActiveGEHandle);
				}
			}
		}
	}
}

void UInventoryItemInstance::TryRemoveEffects(AActor* InOwner)
{
	if(UAbilitySystemComponent* AbilityComponent =  UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
	{
		for(FActiveGameplayEffectHandle ActiveEffectHandle : OngoingEffectHandles)
		{
			if(ActiveEffectHandle.IsValid())
			{
				AbilityComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
			}
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
