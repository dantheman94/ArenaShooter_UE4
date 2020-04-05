// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/BaseGameState.h"
#include "IngameGameState.generated.h"

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

	// Cinematics *****************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Cinematics", Replicated)
		TArray<ACameraCinematic*> _tOpeningCinematics;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Cinematics", Replicated)
		TArray<ACameraCinematic*> _tClosingCinematics;

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Startup ********************************************************************************************************************************

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

public:

	// Cinematics *****************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_AddCinematicToArray(ACameraCinematic* Cinematic);
	
};
