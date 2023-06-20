// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/FootstepsComponent.h"
#include "Character/GD_Characterbase.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/GD_PhysicalMaterial.h"
#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<int32> CVarShowFootsteps(
	TEXT("ShowDebugFootSteps"),
	0,
	TEXT("Draw debug info about footsteps")
	TEXT(" 0: off/n")
	TEXT(" 1: on/n"),
	ECVF_Cheat);

// Sets default values for this component's properties
UFootstepsComponent::UFootstepsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UFootstepsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UFootstepsComponent::HandleFootstep(EFoot Foot)
{
	if(AGD_CharacterBase* Character = Cast<AGD_CharacterBase>(GetOwner()))
	{
		// Debug
		const int32 DebugShowFootsteps = CVarShowFootsteps.GetValueOnAnyThread();
		
		
		if(USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			FHitResult HitResult;
			const FVector SocketLocation = Mesh->GetSocketLocation(Foot == EFoot::Left ? LeftFootSocketName : RightFootSocketName);
			const FVector Location = SocketLocation + FVector::UpVector * 20;

			// Footstep检测
			FCollisionQueryParams QueryParams;
			QueryParams.bReturnPhysicalMaterial = true;
			QueryParams.AddIgnoredActor(Character);
			
			if(GetWorld()->LineTraceSingleByChannel(HitResult , Location , Location + FVector::UpVector * -50.f , ECollisionChannel::ECC_WorldStatic , QueryParams))
			{
				if(HitResult.bBlockingHit)
				{
					if(HitResult.PhysMaterial.Get())
					{
						UGD_PhysicalMaterial* PhysicalMaterial = Cast<UGD_PhysicalMaterial>(HitResult.PhysMaterial.Get());


						if(PhysicalMaterial)
						{
							UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->FootStepSound , Location , 1.f);
						}

						// Debug
						if(DebugShowFootsteps > 0)
						{
							DrawDebugString(GetWorld() , Location , GetNameSafe(PhysicalMaterial) , nullptr , FColor::White , 4.f);
						}
					}
					
					// Debug
					if(DebugShowFootsteps > 0)
					{
						DrawDebugSphere(GetWorld() , Location , 16 , 16 , FColor::Red , false ,4.f);
					}

				}
				else
				{
					// Debug 检测没有命中
					if(DebugShowFootsteps > 0)
					{
						DrawDebugLine(GetWorld() , Location , Location + FVector::UpVector * -50 , FColor::Red
							,false, 4, 0 ,1);
					}
				}
			}
			else
			{
				if(DebugShowFootsteps > 0)
				{
					DrawDebugLine(GetWorld() , Location , Location + FVector::UpVector * -50 , FColor::Red
						,false, 4, 0 ,1);
					DrawDebugSphere(GetWorld() , Location , 16 , 16 , FColor::Red , false ,4.f);
				}
			}

		}
	}
}



