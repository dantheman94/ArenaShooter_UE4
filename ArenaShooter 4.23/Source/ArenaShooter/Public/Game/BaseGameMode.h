// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Structures.h"
#include "TeamComponent.h"

#include "BaseGameMode.generated.h"

// *** CLASSES

class ABasePlayerController;
class ABaseGameState;
class AGameStateBase;
class ABasePlayerState;

UCLASS()
class ARENASHOOTER_API ABaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ABaseGameMode();

protected:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Startup ********************************************************************************************************************************

	/**
	* @summary:	Called when the game starts or when spawned.
	*
	* @return:	virtual void
	*/
	virtual void BeginPlay() override;

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// Lobby Properties ***********************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Lobby Properties")
		int _iMaxLobbySize = 10;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Match Properties")
		bool _bTeamBased = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Lobby")
		TSubclassOf<class UTeamComponent> _TeamComponentClass;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Lobby")
		TArray<class UTeamComponent*> _Teams;

	/*
	*
	*/
	ABaseGameState* _GameState = NULL;

	// Players ********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Lobby")
		TArray<ABasePlayerController*> _ConnectedBasePlayerControllers;
	
public:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Players ********************************************************************************************************************************

	/*
	*
	*/
	virtual void PostLogin(APlayerController* NewPlayer) override;

	///////////////////////////////////////////////

	/*
	*
	*/
	virtual void Logout(AController* ExitingController) override;

	///////////////////////////////////////////////

	/*
	*
	*/
	TArray<ABasePlayerController*>GetConnectedPlayerControllers() { return _ConnectedBasePlayerControllers; }

	// Lobby Properties ***********************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetMaxLobbySize() { return _iMaxLobbySize; }

	///////////////////////////////////////////////

	/*
	*
	*/
	ABaseGameState* GetGameState();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void AddPlayerToTeam(ABasePlayerController* PlayerController, E_TeamNames Team);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void ServerTravel();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		TArray<class UTeamComponent*>GetTeamTemplates() { return _Teams; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void RandomizeTeamPlayers();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void SetNewLobbyHost(FPlayerInfo NewHostInfo);

	// Prematch Setup *************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		void GenerateRandomGamemodeToPlay(FPlaylistInfo Playlist);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void GenerateRandomMapToPlay(FPlaylistInfo Playlist);

	// Match Properties ***********************************************************************************************************************

	/*
	*
	*/
	bool CheckForGameOver();
	
};
