// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASDemoGameMode.h"
#include "GASDemoCharacter.h"
#include "PlayerControllers/GD_PlayerController.h"
#include "UObject/ConstructorHelpers.h"

AGASDemoGameMode::AGASDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_GD_CharacterBase"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerControllerClass = AGD_PlayerController::StaticClass();
}

void AGASDemoGameMode::NotifyPlayerDied(AGD_PlayerController* PlayerController)
{
	if(PlayerController)
	{
		PlayerController->RestartPlayerIn(2.f);
	}
}
