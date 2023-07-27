// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomMapGameMode.h"

#include "Actors/RandomMapActor.h"
#include "Character/GD_CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/GD_PlayerController.h"


ARandomMapGameMode::ARandomMapGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_GD_CharacterBase.BP_GD_CharacterBase_C"));//静态加载 在构造函数中完成
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerControllerClass = AGD_PlayerController::StaticClass();
}

void ARandomMapGameMode::BeginPlay()
{
	/*Super::BeginPlay();
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		if (PlayerController)
		{
			// Spawn the character and possess it by the player controller
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			FTransform Transform;
			AGD_CharacterBase* Character = GetWorld()->SpawnActorDeferred<AGD_CharacterBase>(DefaultPawnClass, Transform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			
			if (Character)
			{
				// Do any setup you need for the character here
				// For example, you can call functions on the character to set its initial state or properties

				// Finish spawning the character
				UGameplayStatics::FinishSpawningActor(Character, FTransform(FRotator::ZeroRotator, FVector::ZeroVector));
				PlayerController->Possess(Character);
			}
		}
	}*/
}



