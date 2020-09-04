// Fill out your copyright notice in the Description page of Project Settings.

#include "HUDMainMenu.h"

#include "BaseGameInstance.h"
#include "UserWidget.h"

// Main Menu ******************************************************************************************************************************

/*
*
*/
void AHUDMainMenu::Transtion()
{
	switch (_CurrentMenuState)
	{
	case E_MainMenuPage::eGT_Splash:
	{
		if (_UI_SplashScreen_Instance != NULL) { _UI_SplashScreen_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenuPage::eGT_MainMenu:
	{
		if (_UI_MainMenu_Instance != NULL) { _UI_MainMenu_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenuPage::eGT_Matchmaking:
	{
		if (_UI_Matchmaking_Instance != NULL) { _UI_Matchmaking_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenuPage::eGT_Playlist:
	{
		if (_UI_Playlist_Instance != NULL) { _UI_Playlist_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenuPage::eGT_ServerBrowser:
	{
		if (_UI_ServerBrowser_Instance != NULL) { _UI_ServerBrowser_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenuPage::eGT_Loading:
	{
		if (_UI_LoadingServer_Instance != NULL) { _UI_LoadingServer_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenuPage::eGT_CreateMatch:
	{
		if (_UI_HostLobby_Instance != NULL) { _UI_HostLobby_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenuPage::eGT_NotHosting:
	{
		if (_UI_ClientLobby_Instance != NULL) { _UI_ClientLobby_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenuPage::eGT_Searching:
	{
		if (_UI_SearchingForMatch_Instance != NULL) { _UI_SearchingForMatch_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenuPage::eGT_PreMatch:
	{
		if (_UI_PreMatch_Instance != NULL) { _UI_PreMatch_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenuPage::eGT_BarracksHome:
	{
		if (_UI_BarracksHome_Instance != NULL) { _UI_BarracksHome_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenuPage::eGT_SettingsHome:
	{
		if (_UI_SettingsHome_Instance != NULL) { _UI_SettingsHome_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenuPage::eGT_SettingsGamepad:
	{
		if (_UI_SettingsGamepad_Instance != NULL) { _UI_SettingsGamepad_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenuPage::eGT_SettingsKeyBindings:
	{
		if (_UI_SettingsKeyBindings_Instance != NULL) { _UI_SettingsKeyBindings_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenuPage::eGT_SettingsDisplay:
	{
		if (_UI_SettingsDisplay_Instance != NULL) { _UI_SettingsDisplay_Instance->RemoveFromParent(); }
		break;
	}
	case E_MainMenuPage::eGT_SettingsAudio:
	{
		if (_UI_SettingsAudio_Instance != NULL) { _UI_SettingsAudio_Instance->RemoveFromParent(); }
		break;
	}
	default: break;
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_GoBack_Implementation()
{
	Transtion();
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_Splash_Implementation()
{
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_Splash;
	
	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_MainMenu_Implementation(int ZOrder)
{
	_PreviousMenuState = _CurrentMenuState;
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_MainMenu;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::HideUI_MainMenu()
{
	// Hide main menu UMG if it exists
	if (_UI_MainMenu_Instance != NULL) { _UI_MainMenu_Instance->RemoveFromParent(); }
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_Matchmaking_Implementation(int ZOrder)
{
	_PreviousMenuState = _CurrentMenuState;
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_Matchmaking;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_Playlist_Implementation(int ZOrder)
{
	_PreviousMenuState = _CurrentMenuState;
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_Playlist;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_ServerBrowser_Implementation(int ZOrder)
{
	_PreviousMenuState = _CurrentMenuState;
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_ServerBrowser;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_HostLobby_Implementation(int ZOrder)
{
	_PreviousMenuState = _CurrentMenuState;
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_CreateMatch;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::HideUI_HostLobby_Implementation()
{
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_CreateMatch;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_ClientLobby_Implementation(int ZOrder)
{
	_PreviousMenuState = _CurrentMenuState;
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_NotHosting;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::HideUI_ClientLobby_Implementation()
{
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_NotHosting;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_SearchingForGames_Implementation(int ZOrder)
{
	_PreviousMenuState = _CurrentMenuState;
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_Searching;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::HideUI_SearchingForGames_Implementation()
{
	// Hide main menu UMG if it exists
	if (_UI_SearchingForMatch_Instance != NULL) { _UI_SearchingForMatch_Instance->RemoveFromParent(); }
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_PreMatchLobby_Implementation(int ZOrder)
{
	_PreviousMenuState = _CurrentMenuState;
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_PreMatch;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::HideUI_PreMatchLobby_Implementation()
{
	// Hide main menu UMG if it exists
	if (_UI_PreMatch_Instance != NULL) { _UI_PreMatch_Instance->RemoveFromParent(); }
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_LobbyRoster_Implementation(int ZOrder)
{}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::HideUI_LobbyRoster_Implementation(bool bHideListOnly)
{
	if (!bHideListOnly)
	{
		// Hide UMG widget if it exists
		if (_UI_LobbyRoster_Instance != NULL) { _UI_LobbyRoster_Instance->RemoveFromParent(); }
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_LoadingServer_Implementation(const FText& Message, int ZOrder)
{
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_Loading;
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_GameModeList_Implementation(int ZOrder)
{

}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_MapList_Implementation(int ZOrder)
{

}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_BarracksHome_Implementation(int ZOrder)
{
	_PreviousMenuState = _CurrentMenuState;
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_BarracksHome;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_SettingsHome_Implementation(int ZOrder)
{
	///_PreviousMenuState = _CurrentMenuState;
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_SettingsHome;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_SettingsGamepad_Implementation(int ZOrder)
{
	_PreviousMenuState = _CurrentMenuState;
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_SettingsGamepad;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_SettingsKeyBindings_Implementation(int ZOrder)
{
	_PreviousMenuState = _CurrentMenuState;
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_SettingsKeyBindings;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_SettingsDisplay_Implementation(int ZOrder)
{
	_PreviousMenuState = _CurrentMenuState;
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_SettingsDisplay;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AHUDMainMenu::ShowUI_SettingsAudio_Implementation(int ZOrder)
{
	_PreviousMenuState = _CurrentMenuState;
	Transtion();
	_CurrentMenuState = E_MainMenuPage::eGT_SettingsAudio;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}

void AHUDMainMenu::ShowUI_NewMenuState_Implementation(E_MainMenuPage NewMenuState, int ZOrder)
{
	_PreviousMenuState = _CurrentMenuState;
	Transtion();
	_CurrentMenuState = NewMenuState;

	UGameInstance* gi = GetWorld()->GetGameInstance();
	checkf(gi, TEXT("Unexpected null game instance!"))
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gi);
		checkf(gameInstance, TEXT("Couldnt cast the GameInstance to a UBaseGameInstance class"))
		{
			gameInstance->SetMenuState(_CurrentMenuState);
		}
	}
}
