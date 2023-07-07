// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/ItemActors/WeaponItemActor.h"

#include "Inventory/InventoryItemInstance.h"


AWeaponItemActor::AWeaponItemActor()
{
	
}

const UWeaponStaticData* AWeaponItemActor::GetWeaponStaticData() const
{
	return ItemInstance ? Cast<UWeaponStaticData>(ItemInstance->GetItemStaticData()) : nullptr;
}

void AWeaponItemActor::InitInternal()
{
	Super::InitInternal();

	// 二选一
	if(const UWeaponStaticData* WeaponStaticData = GetWeaponStaticData())
	{
		if(WeaponStaticData->SkeletalMesh)
		{
			USkeletalMeshComponent* SkeletalMeshComponent = NewObject<USkeletalMeshComponent>(this ,
				USkeletalMeshComponent::StaticClass() , TEXT("MeshComponent"));
			if(SkeletalMeshComponent)
			{
				/** Register this component, creating any rendering/physics state. Will also add itself to the outer Actor's Components array, if not already present. */
				SkeletalMeshComponent->RegisterComponent();
				SkeletalMeshComponent->SetSkeletalMeshAsset(WeaponStaticData->SkeletalMesh);
				SkeletalMeshComponent->AttachToComponent(GetRootComponent() , FAttachmentTransformRules::SnapToTargetNotIncludingScale);

				MeshComponent = SkeletalMeshComponent;
			}
		}else if(WeaponStaticData->StaticMesh)
		{
			UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(this ,
				USkeletalMeshComponent::StaticClass() , TEXT("MeshComponent"));

			StaticMeshComponent->RegisterComponent();
			StaticMeshComponent->SetStaticMesh(WeaponStaticData->StaticMesh);
			StaticMeshComponent->AttachToComponent(GetRootComponent() , FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			
			MeshComponent = StaticMeshComponent;
			
		}
	}
}
