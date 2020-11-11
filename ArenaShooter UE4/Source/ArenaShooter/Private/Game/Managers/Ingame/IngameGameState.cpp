// Fill out your copyright notice in the Description page of Project Settings.

#include "IngameGameState.h"

#include "CameraCinematic.h"
#include "UnrealNetwork.h"

// Startup ********************************************************************************************************************************

void AIngameGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIngameGameState, _tOpeningCinematics);
	DOREPLIFETIME(AIngameGameState, _tClosingCinematics);
}

// Cinematics *****************************************************************************************************************************

/*
*
*/
bool AIngameGameState::Server_Reliable_AddCinematicToArray_Validate(ACameraCinematic* Cinematic)
{ return true; }

void AIngameGameState::Server_Reliable_AddCinematicToArray_Implementation(ACameraCinematic* Cinematic)
{
	if (Cinematic == NULL) { return; }

	switch (Cinematic->GetCinematicType())
	{
	case E_CinematicType::eGT_Opening:
	{
		// No duplicates allowed
		if (!_tOpeningCinematics.Contains(Cinematic))
		{
			// Add to closing cinematic array
			_tOpeningCinematics.Add(Cinematic);
		}
		break;
	}

	case E_CinematicType::eGT_Closing:
	{
		// No duplicates allowed
		if (!_tClosingCinematics.Contains(Cinematic))
		{
			// Add to closing cinematic array
			_tClosingCinematics.Add(Cinematic);
		}
		break;
	}

	default: break;
	}
}

///////////////////////////////////////////////

/*
*
*/
bool AIngameGameState::Server_Reliable_StartOpeningCinematicAllPlayers_Validate()
{ return true; }

void AIngameGameState::Server_Reliable_StartOpeningCinematicAllPlayers_Implementation()
{
	Multicast_Reliable_StartOpeningCinematicAllPlayers();
}

bool AIngameGameState::Multicast_Reliable_StartOpeningCinematicAllPlayers_Validate()
{ return true; }

void AIngameGameState::Multicast_Reliable_StartOpeningCinematicAllPlayers_Implementation()
{
	for (int i = 0; i < _tOpeningCinematics.Num(); i++)
	{
		_tOpeningCinematics[i]->StartCinematic();
	}
}
