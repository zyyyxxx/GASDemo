// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GASDemoGameMode.generated.h"

class AGD_PlayerController;

UCLASS(minimalapi)
class AGASDemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGASDemoGameMode();

	void NotifyPlayerDied(AGD_PlayerController* PlayerController);
};



