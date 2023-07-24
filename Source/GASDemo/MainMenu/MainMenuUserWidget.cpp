// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/MainMenuUserWidget.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

void UMainMenuUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateSessionCompleteDelegate.BindUObject(this, &UMainMenuUserWidget::OnCreateSessionComplete);
	FindSessionsCompleteDelegate.BindUObject(this , &UMainMenuUserWidget::OnFindSessionComplete);
	JoinSessionCompleteDelegate.BindUObject(this,&UMainMenuUserWidget::OnJoinSessionComplete);

	
}

void UMainMenuUserWidget::OnQuitButtonClicked()
{
	// 获取玩家控制器
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		// 执行退出游戏的命令
		UKismetSystemLibrary::QuitGame(this,PlayerController,EQuitPreference::Quit,true);
	}
}

bool UMainMenuUserWidget::OnCreateGameButtonClicked(int PlayerNum , bool bUseLAN)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if(OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{

			auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
			if(ExistingSession != nullptr)
			{
				SessionInterface->DestroySession(NAME_GameSession);
			}

			SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
			
			TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
			SessionSettings->NumPublicConnections = PlayerNum; // 设置最大玩家数
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bIsLANMatch = true;
			SessionSettings->bUsesPresence = false;
			SessionSettings->bUseLobbiesIfAvailable = true;
			SessionSettings->bUsesPresence = true;
			//SessionSettings->Set(FName("MatchType") , FString("FreeForAll") , EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
			

			const ULocalPlayer* LocalPlayer = GetOwningPlayer()->GetLocalPlayer();
			
			bool bWasSuccessful = SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId() , NAME_GameSession , *SessionSettings);
			

			return bWasSuccessful;
		}
	}
	return false;
}

bool UMainMenuUserWidget::OnSearchGameButtonClicked()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if(OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{

			SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);
			
			SessionSearch = MakeShareable(new FOnlineSessionSearch());

			SessionSearch->MaxSearchResults = 20;
			SessionSearch->bIsLanQuery = true;
			//SessionSearch->QuerySettings.Set(FName("MatchType") , FString("FreeForAll") , EOnlineComparisonOp::Equals);
			
			const ULocalPlayer* LocalPlayer = GetOwningPlayer()->GetLocalPlayer();
			bool bWasSuccessfulFind = SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId() , SessionSearch.ToSharedRef());
			FOnlineSessionSearch SearchResult = *SessionSearch;
			return bWasSuccessfulFind;
				
		}
	}
	return false;
	
	
}

void UMainMenuUserWidget::OnJoinGameButtonClicked(FString SessionIdStr)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if(OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
			
			const ULocalPlayer* LocalPlayer = GetOwningPlayer()->GetLocalPlayer();

			for(auto Result : SessionSearch->SearchResults)
			{
				if(SessionIdStr == Result.GetSessionIdStr())
				{
					SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId() , NAME_GameSession , Result);
				}
				
			}
				
		}
	}
}

void UMainMenuUserWidget::OnCreateSessionComplete(FName SessionName , bool bWasSuccessful)
{
	UWorld* World = GetWorld();
	if(World)
	{
		World->ServerTravel(FString("/Game/ThirdPerson/Maps/ThirdPersonMap?listen") , true);
		UE_LOG(LogTemp , Warning , TEXT("%d") , World->GetNetMode());
	}
}

void UMainMenuUserWidget::OnFindSessionComplete(bool bWasSuccessful)
{
	SearchResultNum = SessionSearch->SearchResults.Num();
	for(auto Result : SessionSearch->SearchResults)
	{
		SearchedID.AddUnique(Result.GetSessionIdStr());
		FString Info = Result.Session.OwningUserName + "(" + Result.GetSessionIdStr() + ")";
		
		Info += "Ping: " + FString::FromInt(Result.PingInMs) + " ms " + FString::FromInt(Result.Session.SessionSettings.NumPublicConnections-Result.Session.NumOpenPublicConnections) +
			"/" + FString::FromInt(Result.Session.SessionSettings.NumPublicConnections);
		
		SearchInfo.AddUnique(Info);
	}
	
}

void UMainMenuUserWidget::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if(OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			if(SessionInterface->GetResolvedConnectString(NAME_GameSession , Address))
			{
				APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
				if(PlayerController)
				{
					PlayerController->ClientTravel(Address , TRAVEL_Absolute);
				}
			}
		}
	}
}
