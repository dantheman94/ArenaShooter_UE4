// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseGameState.h"

#include "BaseGameInstance.h"
#include "BaseGameMode.h"
#include "BasePlayerController.h"
#include "BasePlayerState.h"
#include "UnrealNetwork.h"

void ABaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseGameState, _PlayerInfos);
	DOREPLIFETIME(ABaseGameState, _iMaxLobbySize);
	DOREPLIFETIME(ABaseGameState, _Teams);
}

void ABaseGameState::UpdatePlayerList()
{
	// Clients only update their list locally, server replicates new player info
	if (GetLocalRole() != ROLE_Authority) { _OnPlayerInfoRefresh.Broadcast(); }

	// Since the gamemode only exists on the server, then client authority will return NULL :D
	ABaseGameMode* gameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode());
	if (gameMode != NULL)
	{
		// Clear the list, then re-populate it
		_PlayerInfos.Empty();

		// Add an FPlayerInfo for each connected player controller in the gamemode
		for (int i = 0; i < gameMode->GetConnectedPlayerControllers().Num(); i++)
		{
			FPlayerInfo	info;
			ABasePlayerState* playerState = Cast<ABasePlayerState>(gameMode->GetConnectedPlayerControllers()[i]->PlayerState);
			if (playerState != NULL)
			{
				info._PlayerName = playerState->GetPlayerInfo().GetPlayerName();
				info._PlayerTag = playerState->GetPlayerInfo().GetPlayerTag();
				info._bIsHost = playerState->GetPlayerInfo().IsHost();
				info._PlayerState = playerState->GetPlayerInfo().GetPlayerState();
			}
			_PlayerInfos.Add(info);
		}

		// Update player list on all clients connected
		_OnPlayerInfoRefresh.Broadcast();
	}

	// Future-proofing if any extra server code needs to go underneath this (client-authority will hit this instead)
	else { return; }

	UGameInstance* gs = GetGameInstance();
	UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gs);
	if (gameInstance == NULL) { return; }

	// Update team list?
	if (gameInstance->GetTeamBased()) { UpdateTeamList(); }
}

void ABaseGameState::UpdateTeamList()
{
	// Clients only update their list locally, server replicates new team data
	if (GetLocalRole() != ROLE_Authority) { _OnPlayerInfoRefresh.Broadcast(); }

	// Since the gamemode only exists on the server, then client authority will return NULL :D
	ABaseGameMode* gameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode());
	if (gameMode != NULL)
	{
		// Clear the list, then re-populate it
		///_Teams.Empty();
		///
		///for (int i = 0; i < _iLobbyTeamCap; i++)
		///{
		///	if (gameMode->GetTeamTemplates().Num() >= i)
		///	{
		///		_Teams.Add(gameMode->GetTeamTemplates()[i]);
		///	}			
		///}

		// Update player list on all clients connected
		_OnPlayerInfoRefresh.Broadcast();
	}

	// Future-proofing if any extra server code needs to go underneath this (client-authority will hit this instead)
	else { return; }
}

void ABaseGameState::SetMaxLobbySize(int Size)
{
	// Can only be called from the server
	if (GetLocalRole() != ROLE_Authority) { return; }

	_iMaxLobbySize = Size;
}

void ABaseGameState::DisconnectClientFromLobby()
{

}

bool ABaseGameState::Server_Reliable_CheckForGameOver_Validate() { return true; }
void ABaseGameState::Server_Reliable_CheckForGameOver_Implementation()
{

}

bool ABaseGameState::Multicast_Reliable_SetGameMode_Validate(FGameTypeInfo GameTypeInfo) { return true; }
void ABaseGameState::Multicast_Reliable_SetGameMode_Implementation(FGameTypeInfo GameTypeInfo)
{
	// Get reference to game instance
	UGameInstance* gi = GetWorld()->GetGameInstance();
	UBaseGameInstance* baseGameInstance = Cast<UBaseGameInstance>(gi);
	if (baseGameInstance == NULL) { return; }

	// Set gametype info in game instance
	baseGameInstance->SetGameTypeInfo(GameTypeInfo);
}

bool ABaseGameState::Multicast_Reliable_SetMap_Validate(FMapInfo MapInfo) { return true; }
void ABaseGameState::Multicast_Reliable_SetMap_Implementation(FMapInfo MapInfo)
{
	// Get reference to game instance
	UGameInstance* gi = GetWorld()->GetGameInstance();
	UBaseGameInstance* baseGameInstance = Cast<UBaseGameInstance>(gi);
	if (baseGameInstance == NULL) { return; }

	// Set map info in game instance
	baseGameInstance->SetMapInfo(MapInfo);
}

bool ABaseGameState::Server_Reliable_HostHasStartedMatchCountdown_Validate() { return true; }
void ABaseGameState::Server_Reliable_HostHasStartedMatchCountdown_Implementation()
{
	Multicast_Reliable_HostHasStartedMatchCountdown();
}

bool ABaseGameState::Multicast_Reliable_HostHasStartedMatchCountdown_Validate() { return true; }
void ABaseGameState::Multicast_Reliable_HostHasStartedMatchCountdown_Implementation()
{
	_OnMatchCountdown.Broadcast();
}

bool ABaseGameState::Server_Reliable_HostHasCancelledMatchCountdown_Validate() { return true; }
void ABaseGameState::Server_Reliable_HostHasCancelledMatchCountdown_Implementation()
{
	Multicast_Reliable_HostHasCancelledMatchCountdown();
}

bool ABaseGameState::Multicast_Reliable_HostHasCancelledMatchCountdown_Validate() { return true; }
void ABaseGameState::Multicast_Reliable_HostHasCancelledMatchCountdown_Implementation()
{
	_OnMatchCountdownCancel.Broadcast();
}

bool ABaseGameState::Server_Reliable_PromoteToLeader_Validate(FPlayerInfo NewHostPlayerInfo) { return true; }
void ABaseGameState::Server_Reliable_PromoteToLeader_Implementation(FPlayerInfo NewHostPlayerInfo)
{
	// Clear current host
	for (int i = 0; i < _PlayerInfos.Num(); i++)
	{
		ABasePlayerState* playerState = Cast<ABasePlayerState>(_PlayerInfos[i].GetPlayerState());
		if (playerState == NULL) { continue; }

		FPlayerInfo playerInfo = playerState->GetPlayerInfo();
		if (playerInfo.IsHost())
		{
			// Found current host so remove host privileges 
			playerState->Server_Reliable_SetHost(false);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, playerInfo.GetPlayerName() + TEXT(" is no longer the host."));
		}
	}

	// Set new host
	for (int i = 0; i < _PlayerInfos.Num(); i++)
	{
		ABasePlayerState* playerState = Cast<ABasePlayerState>(_PlayerInfos[i].GetPlayerState());
		if (playerState == NULL) { continue; }

		FPlayerInfo playerInfo = playerState->GetPlayerInfo();
		if (playerInfo._PlayerState == NewHostPlayerInfo.GetPlayerState())
		{
			// Player states match so set new host privileges
			playerState->Server_Reliable_SetHost(true);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, playerInfo.GetPlayerName() + TEXT(" is the new host."));
		}
	}

	// Update roster list
	UpdatePlayerList();
}

// Prematch Setup *************************************************************************************************************************

/*
*
*/
bool ABaseGameState::Server_Reliable_GenerateRandomGamemodeToPlay_Validate() { return true; }
void ABaseGameState::Server_Reliable_GenerateRandomGamemodeToPlay_Implementation() { GenerateRandomGamemodeToPlay(); }
void ABaseGameState::GenerateRandomGamemodeToPlay()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		// Get reference to game instance
		UGameInstance* gi = GetWorld()->GetGameInstance();
		UBaseGameInstance* baseGameInstance = Cast<UBaseGameInstance>(gi);
		if (baseGameInstance == NULL) { return; }
 
		// Get playlist
		UDataTable* playlistTable = baseGameInstance->GetPlaylistDataTable();
		FPlaylistInfo* playlist = playlistTable->FindRow<FPlaylistInfo>(TEXT("Debug"), "", true);

		if (playlist != NULL)
		{
			// Get randomized base gamemode from playlist information
			///int gameModeCount = playlist->PlaylistGameTypes.Num();
			int gameModeCount = playlist->_GameTypes.Num();
			int randG = FMath::RandRange(0, gameModeCount - 1);
			E_GameTypes gameMode = playlist->_GameTypes[randG];

			UDataTable* gametypeTable = baseGameInstance->GetGameTypeDataTable();
			if (gametypeTable == NULL) 
				return;

			// Cross reference variant from base gamemode, from playlist
			TArray<TSubclassOf<ABaseGameMode>> variants;
			FGameTypeInfo* gameModeInfo = NULL;
			int size = gametypeTable->GetRowNames().Num();
			for (int i = 0; i < size; i++)
			{
				// Iterate through all known gamemodes
				gameModeInfo = gametypeTable->FindRow<FGameTypeInfo>(FName(*FString::FromInt(i)), "", true);
				if (gameModeInfo != NULL)
				{
					// Matching base gamemode to the random one specified earlier
					if (gameModeInfo->GameType == gameMode)
					{
						// Add variant to the temporary string array
						variants.Add(gameModeInfo->GameMode);
					} else 
						continue;
				}
			}

			if (variants.Num() > 0)
			{
				// Now get a random variant from the temporary variants array
				int randV = FMath::RandRange(0, variants.Num() - 1);
				TSubclassOf<ABaseGameMode> variant = variants[randV];

				// Inform all players of new gamemode
				if (variant != NULL)
				{
					FGameTypeInfo gameType;
					gameType.GameType = gameMode;
					gameType.GameMode = variant;
					gameType.GameTypeName = gameModeInfo->GameTypeName;
					gameType.GameTypeDescription = gameModeInfo->GameTypeDescription;
					gameType.GametypeThumbnail = gameModeInfo->GametypeThumbnail;

					Multicast_Reliable_SetGameMode(gameType);
				}
			}
		}
	}
	else	
		Server_Reliable_GenerateRandomGamemodeToPlay();	
}

/*
*
*/
bool ABaseGameState::Server_Reliable_GenerateRandomMapToPlay_Validate(FPlaylistInfo Playlist) { return true; }
void ABaseGameState::Server_Reliable_GenerateRandomMapToPlay_Implementation(FPlaylistInfo Playlist)
{
	// Get reference to game instance
	UGameInstance* gi = GetWorld()->GetGameInstance();
	UBaseGameInstance* baseGameInstance = Cast<UBaseGameInstance>(gi);
	if (baseGameInstance == NULL) 
		return;

	// Get randomized map from playlist information
	int mapCount = Playlist.PlaylistMaps.Num();
	int rand = FMath::RandRange(0, mapCount - 1);

	// Get map from database based on random integer
	UDataTable* dataBase = baseGameInstance->GetMapDataTable();
	if (dataBase == NULL) { return; }
	FMapInfo* item = dataBase->FindRow<FMapInfo>(FName(*FString::FromInt(rand)), "", true);
	if (item != NULL)
	{
		// Inform all players of new map
		Multicast_Reliable_SetMap(*item);
	} else 
		return;
}

void ABaseGameState::sendmessage_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("bepis"));
}
