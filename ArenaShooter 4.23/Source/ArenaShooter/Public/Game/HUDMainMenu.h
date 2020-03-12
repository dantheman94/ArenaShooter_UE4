// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/UnrealString.h"
#include "GameFramework/HUD.h"

#include "HUDMainMenu.generated.h"

UENUM(BlueprintType)
enum class E_MainMenu : uint8
{
	eGT_Splash UMETA(DisplayName = "Splash Screen"),
	eGT_Loading UMETA(DisplayName = "Loading Screen"),
	eGT_MainMenu UMETA(DisplayName = "Main Menu"),
	eGT_CreateMatch UMETA(DisplayName = "Host Match"),
	eGT_NotHosting UMETA(DisplayName = "Non-host Screen")
};

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API AHUDMainMenu : public AHUD
{
	GENERATED_BODY()

protected:

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// Main Menu ******************************************************************************************************************************

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		E_MainMenu _CurrentMenuState = E_MainMenu::eGT_Splash;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		E_MainMenu _PreviousMenuState = E_MainMenu::eGT_Splash;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_SplashScreen_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Main Menu")
		UUserWidget* _UI_SplashScreen_Instance = NULL;

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
		TSubclassOf<class UUserWidget> _UI_ClientLobby_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Main Menu")
		UUserWidget* _UI_ClientLobby_Instance = NULL;

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

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_LoadingServer_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Main Menu")
		UUserWidget* _UI_LoadingServer_Instance = NULL;

	void Transtion();

public:

	// Main Menu ******************************************************************************************************************************

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
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void ShowUI_PreMatchLobby(bool Hosting, int ZOrder);

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
		void ShowUI_LobbyRoster(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (AutoCreateRefTerm = "TextVarName"))
		void ShowUI_LoadingServer(const FText& Message, int ZOrder);

};
