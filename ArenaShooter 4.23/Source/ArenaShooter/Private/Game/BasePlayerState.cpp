// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerState.h"

#include "BaseGameInstance.h"
#include "BaseGameState.h"
#include "BasePlayerController.h"
#include "UnrealNetwork.h"
#include "HUDMainMenu.h"

void ABasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePlayerState, _PlayerInfo);
}

///////////////////////////////////////////////

/*
*
*/
bool ABasePlayerState::Server_Reliable_GenerateRandomPlayerName_Validate()
{ return true; }

void ABasePlayerState::Server_Reliable_GenerateRandomPlayerName_Implementation()
{
	// Get random player name from game instance
	UGameInstance* gameInstance = (UGameInstance*)GetGameInstance();
	UBaseGameInstance* baseGameInstance = Cast<UBaseGameInstance>(gameInstance);
	if (baseGameInstance != NULL)
	{
		int rand = FMath::RandRange(0, baseGameInstance->GetRandPlayerNameList().Num() - 1);
		_PlayerInfo._PlayerName = baseGameInstance->GetRandPlayerNameList()[rand];
	}

	// Update playerinfo list on all clients
	ABaseGameState* gameState = Cast<ABaseGameState>(GetWorld()->GetGameState());
	if (gameState != NULL) { gameState->UpdatePlayerList(); }
}

///////////////////////////////////////////////

/*
*
*/
bool ABasePlayerState::Server_Reliable_GenerateRandomPlayerTag_Validate()
{ return true; }

void ABasePlayerState::Server_Reliable_GenerateRandomPlayerTag_Implementation()
{
	// Get random player name from game instance
	UGameInstance* gameInstance = (UGameInstance*)GetGameInstance();
	UBaseGameInstance* baseGameInstance = Cast<UBaseGameInstance>(gameInstance);
	if (baseGameInstance != NULL)
	{
		int rand = FMath::RandRange(0, baseGameInstance->GetRandPlayerTagList().Num() - 1);
		_PlayerInfo._PlayerTag = baseGameInstance->GetRandPlayerTagList()[rand];
	}

	// Update playerinfo list on all clients
	ABaseGameState* gameState = Cast<ABaseGameState>(GetWorld()->GetGameState());
	if (gameState != NULL) { gameState->UpdatePlayerList(); }
}

///////////////////////////////////////////////

/*
*
*/
bool ABasePlayerState::Server_Reliable_SetHost_Validate(bool Hosting)
{ return true; }

void ABasePlayerState::Server_Reliable_SetHost_Implementation(bool Hosting)
{
	_PlayerInfo._bIsHost = Hosting;
	OwningClient_Reliable_ShowPreMatchLobby(Hosting);
}

///////////////////////////////////////////////

/*
*
*/
bool ABasePlayerState::Server_Reliable_SetOwningPlayerController_Validate(ABasePlayerController* PlayerController)
{ return true; }

void ABasePlayerState::Server_Reliable_SetOwningPlayerController_Implementation(ABasePlayerController* PlayerController)
{
	_PlayerInfo._PlayerController = PlayerController;
	_PlayerInfo._PlayerState = this;
}

// Main Menu ******************************************************************************************************************************

bool ABasePlayerState::OwningClient_Reliable_ShowUI_HostLobby_Validate(int ZOrder)
{ return true; }

void ABasePlayerState::OwningClient_Reliable_ShowUI_HostLobby_Implementation(int ZOrder)
{
	///UGameInstance* gs = GetGameInstance();
	///UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gs);
	///if (gameInstance == NULL) { return; }
	///
	///gameInstance->HideUI_ClientLobby();
	///gameInstance->ShowUI_HostLobby(ZOrder);
	///gameInstance->ShowUI_LobbyRoster(ZOrder);
}

///////////////////////////////////////////////

bool ABasePlayerState::OwningClient_Reliable_ShowUI_ClientLobby_Validate(int ZOrder)
{ return true; }

void ABasePlayerState::OwningClient_Reliable_ShowUI_ClientLobby_Implementation(int ZOrder)
{
	///UGameInstance* gs = GetGameInstance();
	///UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gs);
	///if (gameInstance == NULL) { return; }
	///
	///gameInstance->HideUI_HostLobby();
	///gameInstance->ShowUI_ClientLobby(ZOrder);
	///gameInstance->ShowUI_LobbyRoster(ZOrder);
}

///////////////////////////////////////////////

bool ABasePlayerState::OwningClient_Reliable_ShowPreMatchLobby_Validate(bool Hosting)
{ return true; }

void ABasePlayerState::OwningClient_Reliable_ShowPreMatchLobby_Implementation(bool Hosting)
{
	ABasePlayerController* controller = _PlayerInfo.GetPlayerController();
	if (controller == NULL) { return; }

	AHUDMainMenu* hud = Cast<AHUDMainMenu>(controller->GetHUD());
	if (hud == NULL) { return; }

	hud->ShowUI_PreMatchLobby(Hosting, 0);
}
