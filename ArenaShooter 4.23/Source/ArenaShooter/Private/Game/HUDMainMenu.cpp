// Fill out your copyright notice in the Description page of Project Settings.

#include "HUDMainMenu.h"
#include "UserWidget.h"

// Main Menu ******************************************************************************************************************************

/*
*
*/
void AHUDMainMenu::Transtion()
{
	switch (_CurrentMenuState)
	{
	case E_MainMenu::eGT_Splash:
	{
		if (_UI_SplashScreen_Instance != NULL) { _UI_SplashScreen_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenu::eGT_MainMenu:
	{
		if (_UI_MainMenu_Instance != NULL) { _UI_MainMenu_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenu::eGT_Loading:
	{
		if (_UI_LoadingServer_Instance != NULL) { _UI_LoadingServer_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenu::eGT_CreateMatch:
	{
		if (_UI_HostLobby_Instance != NULL) { _UI_HostLobby_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenu::eGT_NotHosting:
	{
		if (_UI_ClientLobby_Instance != NULL) { _UI_ClientLobby_Instance->RemoveFromParent(); }
		break;
	}
	default: break;
	}
}

/*
*
*/
void AHUDMainMenu::ShowUI_Splash_Implementation()
{
	Transtion();
	_CurrentMenuState = E_MainMenu::eGT_Splash;
}

/*
*
*/
void AHUDMainMenu::ShowUI_MainMenu_Implementation(int ZOrder)
{
	Transtion();
	_CurrentMenuState = E_MainMenu::eGT_MainMenu;
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_HostLobby_Implementation(int ZOrder)
{
	Transtion();
	_CurrentMenuState = E_MainMenu::eGT_CreateMatch;
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::HideUI_HostLobby_Implementation()
{

}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_ClientLobby_Implementation(int ZOrder)
{
	Transtion();
	_CurrentMenuState = E_MainMenu::eGT_NotHosting;
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::HideUI_ClientLobby_Implementation()
{

}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_LobbyRoster_Implementation(int ZOrder)
{

}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_LoadingServer_Implementation(const FText& Message, int ZOrder)
{
	Transtion();
	_CurrentMenuState = E_MainMenu::eGT_Loading;
}
