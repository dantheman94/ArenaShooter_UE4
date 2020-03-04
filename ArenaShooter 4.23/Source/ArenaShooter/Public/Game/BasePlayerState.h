// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Structures.h"

#include "BasePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// Player Info ****************************************************************************************************************************
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FPlayerInfo _PlayerInfo;

public:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Player Info ****************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_GenerateRandomPlayerName();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_GenerateRandomPlayerTag();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetHost(bool Hosting);

	///////////////////////////////////////////////

	UFUNCTION()
		FPlayerInfo GetPlayerInfo() { return _PlayerInfo; }

};
