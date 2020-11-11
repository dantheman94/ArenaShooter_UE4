// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Managers/BaseGameState.h"
#include "IngameGameState.generated.h"

// *** EVENT DISPATCHERS / DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchComplete);

// *** CLASSES

class ACameraCinematic;

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API AIngameGameState : public ABaseGameState
{
	GENERATED_BODY()

protected:

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// Sequences ******************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Sequences")
		bool _bPlayOpeningCinematic = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Sequences")
		bool _bPlayClosingCinematic = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Sequences")
		bool _bShowBuyMenu = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Sequences", Replicated)
		TArray<ACameraCinematic*> _tOpeningCinematics;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Sequences", Replicated)
		TArray<ACameraCinematic*> _tClosingCinematics;

	// Match **********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Match", Replicated)
		int _iCurrentRoundCount = 1;

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Startup ********************************************************************************************************************************

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

public:

	// Sequences ******************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_AddCinematicToArray(ACameraCinematic* Cinematic);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_StartOpeningCinematicAllPlayers();
	
	/*
	*
	*/
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_Reliable_StartOpeningCinematicAllPlayers();

};
