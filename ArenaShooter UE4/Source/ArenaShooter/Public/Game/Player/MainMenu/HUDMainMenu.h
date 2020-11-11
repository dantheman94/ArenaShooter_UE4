// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Player/BaseHUD.h"

#include "HUDMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API AHUDMainMenu : public ABaseHUD
{
	GENERATED_BODY()

protected:

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// Main Menu ******************************************************************************************************************************

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		E_MainMenuPage _CurrentMenuState = E_MainMenuPage::eGT_Splash;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		E_MainMenuPage _PreviousMenuState = E_MainMenuPage::eGT_Splash;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_SplashScreen_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Main Menu")
		UUserWidget* _UI_SplashScreen_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_MainMenu_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Main Menu")
		UUserWidget* _UI_MainMenu_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_Matchmaking_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Main Menu")
		UUserWidget* _UI_Matchmaking_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_ServerBrowser_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Main Menu")
		UUserWidget* _UI_ServerBrowser_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_Playlist_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Main Menu")
		UUserWidget* _UI_Playlist_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_HostLobby_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Main Menu")
		UUserWidget* _UI_HostLobby_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_ClientLobby_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Main Menu")
		UUserWidget* _UI_ClientLobby_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_SearchingForMatch_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Main Menu")
		UUserWidget* _UI_SearchingForMatch_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_PreMatch_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Main Menu")
		UUserWidget* _UI_PreMatch_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_LobbyRoster_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Main Menu")
		UUserWidget* _UI_LobbyRoster_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_LoadingServer_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Main Menu")
		UUserWidget* _UI_LoadingServer_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_GameModeList_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Main Menu")
		UUserWidget* _UI_GameModeList_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_MapList_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Main Menu")
		UUserWidget* _UI_MapList_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Barracks")
		TSubclassOf<class UUserWidget> _UI_BarracksHome_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Settings")
		UUserWidget* _UI_BarracksHome_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Settings")
		TSubclassOf<class UUserWidget> _UI_SettingsHome_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Settings")
		UUserWidget* _UI_SettingsHome_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Settings")
		TSubclassOf<class UUserWidget> _UI_SettingsGamepad_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Settings")
		UUserWidget* _UI_SettingsGamepad_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Settings")
		TSubclassOf<class UUserWidget> _UI_SettingsKeyBindings_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Settings")
		UUserWidget* _UI_SettingsKeyBindings_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Settings")
		TSubclassOf<class UUserWidget> _UI_SettingsDisplay_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Settings")
		UUserWidget* _UI_SettingsDisplay_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Settings")
		TSubclassOf<class UUserWidget> _UI_SettingsAudio_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Widgets | Settings")
		UUserWidget* _UI_SettingsAudio_Instance = NULL;

public:

	// Main Menu ******************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void Transtion();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		E_MainMenuPage GetCurrentMainMenuState() { return _CurrentMenuState; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_GoBack();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_Splash();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_MainMenu(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void HideUI_MainMenu();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_Matchmaking(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_Playlist(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_ServerBrowser(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void ShowUI_LobbyPrematch(bool Hosting, int ZOrder);

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
		void HideUI_HostLobby();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_ClientLobby(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void HideUI_ClientLobby();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_SearchingForGames(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void HideUI_SearchingForGames();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_PreMatchLobby(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void HideUI_PreMatchLobby();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void ShowUI_LobbyMainMenu(bool Hosting, int ZOrder);

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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void HideUI_LobbyRoster(bool bHideListOnly);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (AutoCreateRefTerm = "TextVarName"))
		void ShowUI_LoadingServer(const FText& Message, int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_GameModeList(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_MapList(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_BarracksHome(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_SettingsHome(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_SettingsGamepad(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_SettingsKeyBindings(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_SettingsDisplay(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_SettingsAudio(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_NewMenuState(E_MainMenuPage NewMenuState, int ZOrder);

};
