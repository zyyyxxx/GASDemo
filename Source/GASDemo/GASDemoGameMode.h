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

	// 无缝切换时需要保持状态的 Actor 列表
	virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) override;

	// 在 Server 端切换地图
	UFUNCTION(BlueprintCallable)
	void ChangeMap(const FString& MapName);
};



