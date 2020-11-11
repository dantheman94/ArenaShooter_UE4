// Fill out your copyright notice in the Description page of Project Settings.

#include "IngamePlayerState.h"

#include "UnrealNetwork.h"

void AIngamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIngamePlayerState, _eSessionState);
	DOREPLIFETIME(AIngamePlayerState, _StartingPrimaryWeapon);
	DOREPLIFETIME(AIngamePlayerState, _StartingSecondaryWeapon);
	DOREPLIFETIME(AIngamePlayerState, _StartingReserveWeapon);
	DOREPLIFETIME(AIngamePlayerState, _StartingGrenadeType);
	DOREPLIFETIME(AIngamePlayerState, _StartingGrenadeCount);
	DOREPLIFETIME(AIngamePlayerState, _StartingEquipment);

}

// Loadout ****************************************************************************************************************************

bool AIngamePlayerState::Server_Reliable_SetStartingPrimaryWeapon_Validate(TSubclassOf<AWeapon> StartingPrimaryWeapon) { return true; }
void AIngamePlayerState::Server_Reliable_SetStartingPrimaryWeapon_Implementation(TSubclassOf<AWeapon> StartingPrimaryWeapon)
{
	_StartingPrimaryWeapon = StartingPrimaryWeapon;
}

bool AIngamePlayerState::Server_Reliable_SetStartingSecondaryWeapon_Validate(TSubclassOf<AWeapon> StartingSecondaryWeapon) { return true; }
void AIngamePlayerState::Server_Reliable_SetStartingSecondaryWeapon_Implementation(TSubclassOf<AWeapon> StartingSecondaryWeapon)
{
	_StartingSecondaryWeapon = StartingSecondaryWeapon;
}

bool AIngamePlayerState::Server_Reliable_SetStartingReserveWeapon_Validate(TSubclassOf<AWeapon> StartingReserveWeapon) { return true; }
void AIngamePlayerState::Server_Reliable_SetStartingReserveWeapon_Implementation(TSubclassOf<AWeapon> StartingReserveWeapon)
{
	_StartingReserveWeapon = StartingReserveWeapon;
}

bool AIngamePlayerState::Server_Reliable_SetStartingGrenadeType_Validate(E_GrenadeType StartingGrenadeType) { return true; }
void AIngamePlayerState::Server_Reliable_SetStartingGrenadeType_Implementation(E_GrenadeType StartingGrenadeType)
{
	_StartingGrenadeType = StartingGrenadeType;
}

bool AIngamePlayerState::Server_Reliable_SetStartingGrenadeCount_Validate(int StartingGrenadeCount) { return true; }
void AIngamePlayerState::Server_Reliable_SetStartingGrenadeCount_Implementation(int StartingGrenadeCount)
{
	_StartingGrenadeCount = StartingGrenadeCount;
}

bool AIngamePlayerState::Server_Reliable_SetStartingEquipment_Validate(E_EquipmentType StartingEquipment) { return true; }
void AIngamePlayerState::Server_Reliable_SetStartingEquipment_Implementation(E_EquipmentType StartingEquipment)
{
	_StartingEquipment = StartingEquipment;
}

// Match **********************************************************************************************************************************

/**
* @summary:
*
* @networking:	Runs on server
*
* @return:	void
*/
bool AIngamePlayerState::Server_Reliable_SetSessionState_Validate(E_SessionState SessionState) { return true; }
void AIngamePlayerState::Server_Reliable_SetSessionState_Implementation(E_SessionState SessionState)
{
	_eSessionState = SessionState;
}
