// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/GD_AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

#include "Net/UnrealNetwork.h"

void UGD_AttributeSet::PostGameplayEffectExecute(const  struct FGameplayEffectModCallbackData& Data)
{
	// PostGameplayEffectExecute仅在instantGE使Attribute的 BaseValue改变时触发
	Super::PostGameplayEffectExecute(Data);
	
	if(Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() , 0.f , GetMaxHealth()));
	}

	//已在角色的委托中设置
	/*else if(Data.EvaluatedData.Attribute == GetMaxMovementSpeedAttribute()) // 设置移动速度
	{
		ACharacter* OwningCharacter = Cast<ACharacter>(GetOwningActor());
		UCharacterMovementComponent* CharacterMovement = OwningCharacter ? OwningCharacter->GetCharacterMovement() : nullptr;

		if(CharacterMovement)
		{
			const float MaxSpeed = GetMaxMovementSpeed();
			
			CharacterMovement->MaxWalkSpeed = MaxSpeed;
		}
	}*/
}

void UGD_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGD_AttributeSet , Health , OldHealth);
}

void UGD_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGD_AttributeSet , MaxHealth , OldMaxHealth);
}

void UGD_AttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGD_AttributeSet , Stamina , OldStamina);
}

void UGD_AttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGD_AttributeSet , MaxStamina , OldMaxStamina);
}

void UGD_AttributeSet::OnRep_MaxMovementSpeed(const FGameplayAttributeData& OldMaxMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGD_AttributeSet , MaxMovementSpeed , OldMaxMovementSpeed);
}



void UGD_AttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGD_AttributeSet , Health , COND_None , REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGD_AttributeSet , MaxHealth , COND_None , REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGD_AttributeSet , Stamina , COND_None , REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGD_AttributeSet , MaxStamina , COND_None , REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGD_AttributeSet , MaxMovementSpeed , COND_None , REPNOTIFY_Always);

}
