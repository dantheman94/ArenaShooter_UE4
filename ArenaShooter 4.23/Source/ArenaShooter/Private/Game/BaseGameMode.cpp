// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseGameMode.h"

#include "BaseGameState.h"
#include "BaseGameInstance.h"
#include "BasePlayerController.h"
#include "BasePlayerState.h"
#include "TeamComponent.h"

// Startup ********************************************************************************************************************************

ABaseGameMode::ABaseGameMode()
{
	bUseSeamlessTravel = false;
}

///////////////////////////////////////////////

void ABaseGameMode::BeginPlay()
{
	ABaseGameState* gameState = Cast<ABaseGameState>(GameState);
	if (gameState != NULL)
	{
		gameState->SetMaxLobbySize(_iMaxLobbySize);
	}

	// Get all team components and add to array for referencing
	TArray<UActorComponent*> comps = GetComponentsByClass(_TeamComponentClass);
	for (int i = 0; i < comps.Num(); i++)
	{
		UTeamComponent* team = Cast<UTeamComponent>(comps[i]);
		if (team != NULL) { _Teams.Add(team); }
	}
}

///////////////////////////////////////////////

/*
*
*/
void ABaseGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// If the joining player is a lobby player controller, add it to the list of connected players
	ABasePlayerController* joiningPlayer = Cast<ABasePlayerController>(NewPlayer);
	if (joiningPlayer != NULL)
	{
		// Generate playerinfo data (these functions will update the playerinfo list in the gamestate themselves)
		ABasePlayerState* playerState = Cast<ABasePlayerState>(NewPlayer->PlayerState);
		if (playerState != NULL)
		{
			playerState->Server_Reliable_GenerateRandomPlayerName();
			playerState->Server_Reliable_GenerateRandomPlayerTag();
			playerState->Server_Reliable_SetHost(_ConnectedBasePlayerControllers.Num() == 0);
			playerState->Server_Reliable_SetOwningPlayerController(joiningPlayer);
		}

		_ConnectedBasePlayerControllers.Add(joiningPlayer);
	}
}

///////////////////////////////////////////////

/*
*
*/
void ABaseGameMode::Logout(AController* ExitingController)
{
	Super::Logout(ExitingController);

	// Update the connected players array and the playerinfo list whenever a player leaves
	ABasePlayerController* leavingPlayer = Cast<ABasePlayerController>(ExitingController);
	if (leavingPlayer != NULL)
	{
		_ConnectedBasePlayerControllers.Remove(leavingPlayer);

		ABaseGameState* gameState = Cast<ABaseGameState>(GameState);
		if (gameState != NULL)
		{
			gameState->UpdatePlayerList();
		}
	}
}

// Lobby Properties ***********************************************************************************************************************

/*
*
*/
ABaseGameState* ABaseGameMode::GetGameState()
{
	if (_GameState != NULL) { return _GameState; } else
	{
		// Get GameState and cast it to the correct type
		AGameStateBase* gs = GetWorld()->GetGameState();
		ABaseGameState* gameState = Cast<ABaseGameState>(gs);
		if (gameState != NULL)
		{
			_GameState = gameState;
			return _GameState;
		}
	}

	return NULL;
}

///////////////////////////////////////////////

/*
*
*/
void ABaseGameMode::AddPlayerToTeam(ABasePlayerController* PlayerController, E_TeamNames Team)
{
	// Find team in list
	UTeamComponent* team = NULL;
	for (int i = 0; i < _Teams.Num(); i++)
	{
		if (_Teams[i]->GetName() == Team)
		{
			team = _Teams[i];
			break;
		}
	}

	// Sanity check
	if (team == NULL) { return; }

	// Add to team
	team->AddPlayerToTeam(PlayerController);
}

// Prematch Setup *************************************************************************************************************************

/*
*
*/
void ABaseGameMode::GenerateRandomGamemodeToPlay(FPlaylistInfo Playlist)
{
	// Get reference to game instance
	UGameInstance* gi = GetWorld()->GetGameInstance();
	UBaseGameInstance* baseGameInstance = Cast<UBaseGameInstance>(gi);
	if (baseGameInstance == NULL) { return; }

	// Get randomized base gamemode from playlist information
	int gameModeCount = Playlist.PlaylistGameTypes.Num();
	int randG = FMath::RandRange(0, gameModeCount - 1);
	E_GameTypes gameMode = Playlist.PlaylistGameTypes[randG];

	// Get gamemode variant from game instance
	UDataTable* dataBase = baseGameInstance->GetGameTypeDataTable();
	if (dataBase == NULL) { return; }

	// Cross reference variant from base gamemode, from playlist
	TArray<TSubclassOf<ABaseGameMode>> variants;
	FGameTypeInfo* gameModeInfo = NULL;
	int size = dataBase->GetRowNames().Num();
	for (int i = 0; i < size; i++)
	{
		// Iterate through all known gamemodes
		gameModeInfo = dataBase->FindRow<FGameTypeInfo>(FName(*FString::FromInt(i)), "", true);
		if (gameModeInfo != NULL)
		{
			// Matching base gamemode to the random one specified earlier
			if (gameModeInfo->GameType == gameMode)
			{
				// Add variant to the temporary string array
				variants.Add(gameModeInfo->GameMode);
			}
			else continue;
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

			if (GetGameState() != NULL) { GetGameState()->Multicast_Reliable_SetGameMode(gameType); }
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void ABaseGameMode::GenerateRandomMapToPlay(FPlaylistInfo Playlist)
{
	// Get reference to game instance
	UGameInstance* gi = GetWorld()->GetGameInstance();
	UBaseGameInstance* baseGameInstance = Cast<UBaseGameInstance>(gi);
	if (baseGameInstance == NULL) { return; }
	
	// Get randomized map from playlist information
	int mapCount = Playlist.PlaylistMaps.Num();
	int rand = FMath::RandRange(0, mapCount - 1);

	// Get map from database based on random integer
	UDataTable* dataBase = baseGameInstance->GetGameTypeDataTable();
	if (dataBase == NULL) { return; }
	FMapInfo* item = dataBase->FindRow<FMapInfo>(FName(*FString::FromInt(rand)), "", true);
	if (item != NULL)
	{
		// Inform all players of new map
		if (GetGameState() != NULL) { GetGameState()->Multicast_Reliable_SetMap(*item); }
	}
	else { return; }
}

// Match Properties ***********************************************************************************************************************

/*
*
*/
bool ABaseGameMode::CheckForGameOver()
{
	// Only run on server
	if (Role != ROLE_Authority) { return false; }

	return false;
}
