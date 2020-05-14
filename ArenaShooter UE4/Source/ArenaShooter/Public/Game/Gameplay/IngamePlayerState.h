// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/BasePlayerState.h"
#include "Structures.h"
#include "IngamePlayerState.generated.h"

class AInteractable;

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API AIngamePlayerState : public ABasePlayerState
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

	// Loadout ********************************************************************************************************************************

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		TSubclassOf<AInteractable> _StartingPrimaryWeapon = NULL;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		TSubclassOf<AInteractable> _StartingSecondaryWeapon = NULL;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		TSubclassOf<AInteractable> _StartingReserveWeapon = NULL;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		E_GrenadeType _StartingGrenadeType = E_GrenadeType::eWBT_Frag;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		int _StartingGrenadeCount = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		E_EquipmentType _StartingEquipment = E_EquipmentType::eWBT_TripMine;

	// Match **********************************************************************************************************************************

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		E_SessionState _eSessionState = E_SessionState::eSS_Default;

public:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Loadout ********************************************************************************************************************************

	/**
	* @summary:
	*
	* @networking:	Runs on server
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_SetStartingPrimaryWeapon(TSubclassOf<AInteractable> StartingPrimaryWeapon);

	/**
	* @summary:
	*
	* @networking:	Runs on server
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_SetStartingSecondaryWeapon(TSubclassOf<AInteractable> StartingSecondaryWeapon);

	/**
	* @summary:
	*
	* @networking:	Runs on server
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_SetStartingReserveWeapon(TSubclassOf<AInteractable> StartingReserveWeapon);

	/**
	* @summary:
	*
	* @networking:	Runs on server
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_SetStartingGrenadeType(E_GrenadeType StartingGrenadeType);

	/**
	* @summary:
	*
	* @networking:	Runs on server
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_SetStartingGrenadeCount(int StartingGrenadeCount);

	/**
	* @summary:
	*
	* @networking:	Runs on server
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_SetStartingEquipment(E_EquipmentType StartingEquipment);

	// Match **********************************************************************************************************************************

	/**
	* @summary:
	*
	* @networking:	Runs on server
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_SetSessionState(E_SessionState SessionState);
};
