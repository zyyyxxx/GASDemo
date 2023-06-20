// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_Step.h"
#include "Character/GD_CharacterBase.h"
#include "ActorComponents/FootstepsComponent.h"

void UAnimNotify_Step::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	check(MeshComp);

	AGD_CharacterBase* Character = MeshComp ? Cast<AGD_CharacterBase>(MeshComp->GetOwner()) : nullptr;

	if(Character)
	{
		if(UFootstepsComponent* FootstepsComponent = Character->GetFootstepComponent())
		{
			FootstepsComponent->HandleFootstep(Foot);
		}
		
	}
}
