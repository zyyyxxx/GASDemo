// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "MainMenuUserWidget.generated.h"

class FOnlineSessionSearch;
/**
 * 
 */
UCLASS()
class GASDEMO_API UMainMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	int SearchResultNum = -1;

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> SearchID;

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> User;
	
	virtual void NativeConstruct() override;


	UFUNCTION(BlueprintCallable)
	void OnQuitButtonClicked();

	UFUNCTION(BlueprintCallable)
	bool OnCreateGameButtonClicked( int PlayerNum , bool bUseLan);
	
	UFUNCTION(BlueprintCallable)
	bool OnSearchGameButtonClicked();

	void OnCreateSessionComplete(FName SessionName , bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	
private:
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
};
