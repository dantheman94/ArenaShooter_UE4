// Fill out your copyright notice in the Description page of Project Settings.

#include "IngameGameMode.h"

#include "BasePlayerController.h"
#include "IngamePlayerState.h"
#include "Kismet//KismetMathLibrary.h"
#include "SpawnVolume.h"
#include "Weapon.h"

// Match **********************************************************************************************************************************

/*
*
*/
void AIngameGameMode::MatchComplete(TArray<ABasePlayerController*> WinningControllers, TArray<ABasePlayerController*> LosingControllers)
{

}

/*
*
*/
void AIngameGameMode::InitialPlayerSpawn(ABasePlayerController* PlayerController)
{
	// Spawn player pawn based on team
	AIngamePlayerState* playerState = Cast<AIngamePlayerState>(PlayerController->PlayerState);
	if (playerState == NULL) { return; }

	// Get random starting spawn point
	FTeamSpawnVolumes teamSpawnInfos;
	for (int i = 0; i < _TeamSpawnVolumes.Num(); i++)
	{
		// Matching team
		if (playerState->GetAssignedTeam() == _TeamSpawnVolumes[i].GetTeam())
		{ teamSpawnInfos = _TeamSpawnVolumes[i]; break; }
	}

	if (teamSpawnInfos.GetStartingSpawnVolumes().Num() == 0) { return; }
	int rand = UKismetMathLibrary::RandomIntegerInRange(0, teamSpawnInfos.GetStartingSpawnVolumes().Num() - 1);
	
	ASpawnVolume* spawnVolume = teamSpawnInfos.GetStartingSpawnVolumes()[rand];
	FVector spawnLocation = spawnVolume->GetRandomLocationWithinVolumeBounds();
	if (!spawnLocation.Equals(FVector::ZeroVector))
	{
		// Apply an offset so that our players don't spawn in the ground
		FVector vec = FVector(spawnLocation.X, spawnLocation.Y, spawnLocation.Z + 300.0f);
		FRotator rot = UKismetMathLibrary::MakeRotFromX(spawnVolume->GetActorForwardVector());
		FTransform spawnTransform = FTransform(rot, vec, FVector(1.0f, 1.0f, 1.0f));

		// Spawn new pawn
		FActorSpawnParameters spawnInfo;
		spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		APawn* newPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, spawnTransform, spawnInfo);
	
		// Destroy old pawn
		APawn* oldPawn = PlayerController->GetPawn();
		PlayerController->UnPossess();
		PlayerController->Possess(newPawn);
	
	}
}

/*
*
*/
void AIngameGameMode::CollectActorGarbage(AActor* ActorToCollect)
{
	if (ActorToCollect != NULL) { ActorToCollect->Destroy(); }
}

// Spawn Volumes **************************************************************************************************************************

/*
*
*/
void AIngameGameMode::RegisterSpawnVolumeWithGametype(ASpawnVolume* SpawnVolume)
{
	bool match = false;
	for (int i = 0; i < SpawnVolume->GetGameModes().Num(); i++)
	{
		if (_Gametype == SpawnVolume->GetGameModes()[i])
		{
			match = true;
			break;
		}
	}
	if (!match) { return; }

	// Add to all level spawn volumes array
	// No duplicates
	if (_LevelSpawnVolumes.Contains(SpawnVolume)) { return; } 
	else { _LevelSpawnVolumes.Add(SpawnVolume); }

	// If the team already exists in the gamemode array references
	bool alreadyExists = false;
	for (int j = 0; j < _TeamSpawnVolumes.Num(); j++)
	{
		if (_TeamSpawnVolumes[j].GetTeam() == SpawnVolume->GetTeam())
		{
			// Team match found
			FTeamSpawnVolumes vol = _TeamSpawnVolumes[j];

			// Ensure no duplicates
			if (vol.GetStartingSpawnVolumes().Contains(SpawnVolume)) { return; }
			if (vol.GetRespawnVolumes().Contains(SpawnVolume)) { return; }

			// Add to array
			if (SpawnVolume->IsStartingspawnVolume())
			{ vol.AddStartingSpawnVolume(SpawnVolume); } else
			{ vol.AddRespawnVolume(SpawnVolume); }

			alreadyExists = true;
			break;
		}
	}

	if (!alreadyExists)
	{
		// Team doesn't exist so create a new struct entry in the array
		FTeamSpawnVolumes newEntry = FTeamSpawnVolumes();
		newEntry.SetTeam(SpawnVolume->GetTeam());
		if (SpawnVolume->IsStartingspawnVolume())
		{ newEntry.AddStartingSpawnVolume(SpawnVolume); } else
		{ newEntry.AddRespawnVolume(SpawnVolume); }
	}
}
