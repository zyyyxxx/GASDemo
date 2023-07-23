// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/MainMenuPlayerController.h"

#include "Blueprint/UserWidget.h"

AMainMenuPlayerController::AMainMenuPlayerController()
{
	
}

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if(IsLocalController())
	{
		TSubclassOf<UUserWidget> MainMenuWidgetClass = LoadClass<UUserWidget>(this,TEXT("/Game/MainMenu/Blueprints/WBP_MainMenu.WBP_MainMenu_C"));
		UUserWidget* MainMenuWidget = CreateWidget(this ,MainMenuWidgetClass);
		MainMenuWidget->AddToViewport();
		SetShowMouseCursor(true);
	}
}
