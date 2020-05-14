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
			playerState->Server_Reliable_SetPlayerController(joiningPlayer);
			playerState->Server_Reliable_SetHost(_ConnectedBasePlayerControllers.Num() == 0);
		}

		_ConnectedBasePlayerControllers.Add(joiningPlayer);

		ABaseGameState* gameState = Cast<ABaseGameState>(GameState);
		if (gameState != NULL)
		{
			gameState->UpdatePlayerList();
		}

		// Add them to a team if needed
		UGameInstance* gs = GetGameInstance();
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(gs);
		if (gameInstance == NULL) { return; }
		if (gameInstance->GetTeamBased())
		{
			// Find first available space in any team
			for (int i = 0; i < _Teams.Num(); i++)
			{
				if (_Teams[i]->GetPlayers().Num() < _Teams[i]->GetMaxTeamSize())
				{
					// Add player to team
					E_TeamNames team = _Teams[i]->GetName();
					AddPlayerToTeam(joiningPlayer, team);
					break;
				}
			}
		}
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

///////////////////////////////////////////////

/*
*
*/
void ABaseGameMode::ServerTravel()
{
	// Get reference to game instance
	UGameInstance* gi = GetWorld()->GetGameInstance();
	UBaseGameInstance* baseGameInstance = Cast<UBaseGameInstance>(gi);
	if (baseGameInstance == NULL) { return; }

	baseGameInstance->ServerTravel();
}

///////////////////////////////////////////////

/*
*
*/
void ABaseGameMode::RandomizeTeamPlayers()
{
	// For all connected players
	for (int i = 0; i < _ConnectedBasePlayerControllers.Num(); i++)
	{
		// Find first available space in any team
		for (int j = 0; j < _Teams.Num(); j++)
		{
			if (_Teams[j]->GetPlayers().Num() < _Teams[j]->GetMaxTeamSize())
			{
				// Add player to team
				E_TeamNames team = _Teams[i]->GetName();
				AddPlayerToTeam(_ConnectedBasePlayerControllers[i], team);
				break;
			}
		}
	}

	if (GetGameState() != NULL) { GetGameState()->UpdatePlayerList(); }
}

// Match Properties ***********************************************************************************************************************

/*
*
*/
bool ABaseGameMode::CheckForGameOver()
{
	// Only run on server
	if (GetLocalRole() != ROLE_Authority) { return false; }

	return false;
}
