// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASDemoGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/GD_PlayerController.h"
#include "UObject/ConstructorHelpers.h"

AGASDemoGameMode::AGASDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_GD_CharacterBase"));//静态加载 在构造函数中完成
	if (PlayerPawnBPClass.Class != NULL)
	{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
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

void AGASDemoGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);
	
	// Add the main player controller to the actor list
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		ActorList.Add(PlayerController);
	}
}

void AGASDemoGameMode::ChangeMap(const FString& MapName)
{
	GetWorld()->ServerTravel(MapName);
}
