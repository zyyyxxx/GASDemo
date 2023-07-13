// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/ItemActors/WeaponItemActor.h"

#include "Inventory/InventoryItemInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/GD_PhysicalMaterial.h"
#include "kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

AWeaponItemActor::AWeaponItemActor()
{
	
}

const UWeaponStaticData* AWeaponItemActor::GetWeaponStaticData() const
{
	return ItemInstance ? Cast<UWeaponStaticData>(ItemInstance->GetItemStaticData()) : nullptr;
}

FVector AWeaponItemActor::GetMuzzleLocation() const
{
	return MeshComponent ? MeshComponent->GetSocketLocation(TEXT("Muzzle")) : GetActorLocation() ;  
}

void AWeaponItemActor::PlayweaponEffects(const FHitResult& InHitResult)
{
	if(HasAuthority())
	{
		// 服务端触发 发送多播
		MulticastPlayweaponEffects(InHitResult);
	}else
	{
		//ServerPlayweaponEffects_Implementation(InHitResult);
		PlayweaponEffectsInternal(InHitResult);
	}
}

void AWeaponItemActor::ServerPlayweaponEffects_Implementation(const FHitResult& InHitResult)
{
	PlayweaponEffectsInternal(InHitResult);
}


void AWeaponItemActor::MulticastPlayweaponEffects_Implementation(const FHitResult& InHitResult)
{
	// Multicast被客户端接收到，需要保证不是客户端的自己控制的角色，因为在PlayweaponEffects已被触发过一次，不需要重复触发
	if(!Owner || Owner->GetLocalRole() != ROLE_AutonomousProxy) 
	{
		PlayweaponEffectsInternal(InHitResult);
	}
}

void AWeaponItemActor::PlayweaponEffectsInternal(const FHitResult& InHitResult)
{
	if(InHitResult.PhysMaterial.Get())
	{
		// 使用自定义的PhysMatrtial
		UGD_PhysicalMaterial* PhysicalMaterial = Cast<UGD_PhysicalMaterial>(InHitResult.PhysMaterial.Get());

		if(PhysicalMaterial)
		{
			UGameplayStatics::PlaySoundAtLocation(this , PhysicalMaterial->PointImpactSound ,
				InHitResult.ImpactPoint , 1.f);

			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PhysicalMaterial->PointImpactVFX ,
				InHitResult.ImpactPoint);
		}
	}
	if(const UWeaponStaticData* WeaponStaticData = GetWeaponStaticData())
	{
		UGameplayStatics::PlaySoundAtLocation(this , WeaponStaticData->AttackSound ,
			GetActorLocation() , 1.f);
	}
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
