// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/BaseGameMode.h"
#include "Structures.h"

#include "IngameGameMode.generated.h"

class ABasePlayerController;
class ASpawnVolume;
class AWeapon;

USTRUCT(BlueprintType)
struct FTeamSpawnVolumes
{
	GENERATED_BODY()

		FTeamSpawnVolumes() {}

public:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		E_TeamNames _Team;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		TArray<ASpawnVolume*> _StartingSpawnVolumes;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		TArray<ASpawnVolume*> _RespawnVolumes;

	void SetTeam(E_TeamNames Team) { _Team = Team; }

	void AddStartingSpawnVolume(ASpawnVolume* SpawnVolume) { _StartingSpawnVolumes.Add(SpawnVolume); }
	
	void AddRespawnVolume(ASpawnVolume* SpawnVolume) { _RespawnVolumes.Add(SpawnVolume); }

	E_TeamNames GetTeam() { return _Team; }

	TArray<ASpawnVolume*> GetStartingSpawnVolumes() { return _StartingSpawnVolumes; }

	TArray<ASpawnVolume*> GetRespawnVolumes() { return _RespawnVolumes; }

};

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API AIngameGameMode : public ABaseGameMode
{
	GENERATED_BODY()

protected:

	// Match **********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Match")
		E_GameTypes _Gametype = E_GameTypes::eGT_TDM;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Match")
		int _iMaxRoundCount = 1;

	// Loadout ********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Loadout")
		AWeapon* _LoadoutPrimaryWeapon = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Loadout")
		AWeapon* _LoadoutReserveWeapon = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Loadout")
		E_GrenadeType _LoadoutGrenadeType = E_GrenadeType::eWBT_Frag;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Loadout")
		int _LoadoutGrenadeCount = 1;


	// Spawn Volumes **************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Spawn Volumes")
		TArray<ASpawnVolume*> _LevelSpawnVolumes;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Spawn Volumes")
		TArray<FTeamSpawnVolumes> _TeamSpawnVolumes;

public:

	// Match **********************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Match")
		E_GameTypes GetGameType() { return _Gametype; }

	/*
	*
	*/
	UFUNCTION()
		void MatchComplete(TArray<ABasePlayerController*> WinningControllers, TArray<ABasePlayerController*> LosingControllers);

	/*
	*
	*/
	UFUNCTION()
		void InitialPlayerSpawn(ABasePlayerController* PlayerController);

	/*
	*
	*/
	UFUNCTION()
		void CollectActorGarbage(AActor* ActorToCollect);

	// Spawn Volumes **************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, Category = "Spawn Volumes")
		void RegisterSpawnVolumeWithGametype(ASpawnVolume* SpawnVolume);

};
