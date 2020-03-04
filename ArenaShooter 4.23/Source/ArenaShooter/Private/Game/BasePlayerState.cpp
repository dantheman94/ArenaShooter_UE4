// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerState.h"

#include "BaseGameInstance.h"
#include "BaseGameState.h"

// ****************************************************************************************************************************************
// ************************************ FUNCTIONS *****************************************************************************************
// ****************************************************************************************************************************************

// Player Info ****************************************************************************************************************************

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
void ABasePlayerState::Server_Reliable_SetHost_Implementation(bool Hosting)
{
	_PlayerInfo._bIsHost = Hosting;
}

bool ABasePlayerState::Server_Reliable_SetHost_Validate(bool Hosting)
{ return true; }
