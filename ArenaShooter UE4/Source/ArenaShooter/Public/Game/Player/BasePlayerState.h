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

public:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

protected:

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// Player Info ****************************************************************************************************************************
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		FPlayerInfo _PlayerInfo;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
		E_TeamNames _eAssignedTeam = E_TeamNames::eTN_Alpha;

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

	/*
	*
	*/
	UFUNCTION()
		void SetPlayerController(ABasePlayerController* PlayerController);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetPlayerController(ABasePlayerController* PlayerController);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		FPlayerInfo GetPlayerInfo() { return _PlayerInfo; }

	///////////////////////////////////////////////

	UFUNCTION(BlueprintPure)
		E_TeamNames GetAssignedTeam() { return _eAssignedTeam; }

	// Main Menu ******************************************************************************************************************************

	UFUNCTION(Client, Reliable, WithValidation, BlueprintCallable)
		void OwningClient_Reliable_ShowUI_HostLobby(int ZOrder);

	///////////////////////////////////////////////

	UFUNCTION(Client, Reliable, WithValidation, BlueprintCallable)
		void OwningClient_Reliable_ShowUI_ClientLobby(int ZOrder);

	///////////////////////////////////////////////

	UFUNCTION(Client, Reliable, WithValidation, BlueprintCallable)
		void OwningClient_Reliable_ShowLobbyPreMatch(bool Hosting);

	UFUNCTION(Client, Reliable, WithValidation, BlueprintCallable)
		void OwningClient_Reliable_ShowLobbyMainMenu(bool Hosting);

};
