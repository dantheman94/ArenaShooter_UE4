// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BaseGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// Main Menu ******************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_MainMenu_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Main Menu")
		UUserWidget* _UI_MainMenu_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_HostLobby_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Main Menu")
		UUserWidget* _UI_HostLobby_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_LobbyRoster_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Main Menu")
		UUserWidget* _UI_LobbyRoster_Instance = NULL;

	// Random Names ****************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Random Generator | Player Names")
		TArray<FString> _RandPlayerNameList;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Random Generator | Player Names")
		TArray<FString> _RandPlayerTagList;

public:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Main Menu ******************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_MainMenu();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_HostLobby(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_LobbyRoster(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		TArray<FString> GetRandPlayerNameList() { return _RandPlayerNameList; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		TArray<FString> GetRandPlayerTagList() { return _RandPlayerTagList; }

};
