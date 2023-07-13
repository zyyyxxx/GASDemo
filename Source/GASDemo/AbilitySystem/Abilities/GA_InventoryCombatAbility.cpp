// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_InventoryCombatAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/GD_CharacterBase.h"
#include "Inventory/ItemActors/WeaponItemActor.h"
#include "Kismet/KismetSystemLibrary.h"

FGameplayEffectSpecHandle UGA_InventoryCombatAbility::GetWeaponEffectSpec(const FHitResult& HitResult)
{
	if(UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
	{
		if(const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

			FGameplayEffectSpecHandle OutSpec = AbilitySystemComponent->MakeOutgoingSpec(
				WeaponStaticData->DamageEffect , 1 , EffectContext);

			//Sets a gameplay tag Set By Caller magnitude value
			// 使用Tag caller modifier 来 修改 health（GE的蓝图中 Modifier Magnitude 设置为Set by Caller）
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(OutSpec ,
				FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")) , -WeaponStaticData->BaseDamage);

			return OutSpec;
		}
	}
	return FGameplayEffectSpecHandle();
}

const bool UGA_InventoryCombatAbility::GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType,
	FHitResult& OutHitResult)
{
	AWeaponItemActor* WeaponItemActor = GetEquippedWeaponItemActor();

	AGD_CharacterBase* Character = GetGD_CharacterFromActorInfo();

	const FTransform CameraTransform = Character->GetFollowCamera()->GetComponentTransform();

	const FVector FocusTraceEnd = CameraTransform.GetLocation() + CameraTransform.GetRotation().Vector() * TraceDistance;

	TArray<AActor*> ActorToIgnore = {GetAvatarActorFromActorInfo()};

	FHitResult FocusHit;

	// 第一个射线检测，从摄像机视角发射
	UKismetSystemLibrary::LineTraceSingle(this , CameraTransform.GetLocation() ,FocusTraceEnd ,
		TraceType ,false , ActorToIgnore , EDrawDebugTrace::None , FocusHit , true);
	
	FVector MuzzleLocation = WeaponItemActor->GetMuzzleLocation();

	const FVector WeaponTraceEnd = MuzzleLocation + (FocusHit.Location - MuzzleLocation).GetSafeNormal() * TraceDistance;

	// 第二个射线检测， 从枪口出发，判断是否命中物体
	UKismetSystemLibrary::LineTraceSingle(this , MuzzleLocation , WeaponTraceEnd ,
		TraceType ,false , ActorToIgnore , EDrawDebugTrace::None , OutHitResult , true);
	
	return OutHitResult.bBlockingHit;
}
