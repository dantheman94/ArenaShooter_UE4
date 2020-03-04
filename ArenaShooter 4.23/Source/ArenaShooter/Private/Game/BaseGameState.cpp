// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameState.h"

#include "BaseGameMode.h"
#include "BasePlayerController.h"
#include "BasePlayerState.h"
#include "UnrealNetwork.h"


void ABaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseGameState, _PlayerInfos);
	DOREPLIFETIME(ABaseGameState, _iMaxLobbySize);
}

///////////////////////////////////////////////

void ABaseGameState::UpdatePlayerList()
{
	// Only proceed if this is being called from the server
	if (Role != ROLE_Authority) { return; }

	// Clear the list, then re-populate it
	_PlayerInfos.Empty();

	ABaseGameMode* gameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode());
	if (gameMode != NULL)
	{
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
			}
			_PlayerInfos.Add(info);
		}

		_OnPlayerInfoRefresh.Broadcast();
	}
}

///////////////////////////////////////////////

void ABaseGameState::SetMaxLobbySize(int Size)
{
	// Can only be called from the server
	if (Role != ROLE_Authority) { return; }

	_iMaxLobbySize = Size;
}
