// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameMode.h"

#include "BaseGameState.h"
#include "BasePlayerController.h"
#include "BasePlayerState.h"

ABaseGameMode::ABaseGameMode()
{
	bUseSeamlessTravel = true;
}

void ABaseGameMode::BeginPlay()
{
	ABaseGameState* gameState = Cast<ABaseGameState>(GameState);
	if (gameState != NULL)
	{
		gameState->SetMaxLobbySize(_iMaxLobbySize);
	}
}

///////////////////////////////////////////////

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
