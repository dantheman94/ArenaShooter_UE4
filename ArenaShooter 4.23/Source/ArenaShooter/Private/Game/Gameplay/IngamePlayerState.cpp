// Fill out your copyright notice in the Description page of Project Settings.

#include "IngamePlayerState.h"

#include "UnrealNetwork.h"

void AIngamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIngamePlayerState, _StartingPrimaryWeapon);
	DOREPLIFETIME(AIngamePlayerState, _StartingSecondaryWeapon);
	DOREPLIFETIME(AIngamePlayerState, _StartingReserveWeapon);
	DOREPLIFETIME(AIngamePlayerState, _StartingGrenadeType);
	DOREPLIFETIME(AIngamePlayerState, _StartingGrenadeCount);
	DOREPLIFETIME(AIngamePlayerState, _StartingEquipment);

}

// Loadout ****************************************************************************************************************************

bool AIngamePlayerState::Server_Reliable_SetStartingPrimaryWeapon_Validate(TSubclassOf<AInteractable> StartingPrimaryWeapon)
{ return true; }

void AIngamePlayerState::Server_Reliable_SetStartingPrimaryWeapon_Implementation(TSubclassOf<AInteractable> StartingPrimaryWeapon)
{
	_StartingPrimaryWeapon = StartingPrimaryWeapon;
}

bool AIngamePlayerState::Server_Reliable_SetStartingSecondaryWeapon_Validate(TSubclassOf<AInteractable> StartingSecondaryWeapon)
{ return true; }

void AIngamePlayerState::Server_Reliable_SetStartingSecondaryWeapon_Implementation(TSubclassOf<AInteractable> StartingSecondaryWeapon)
{
	_StartingSecondaryWeapon = StartingSecondaryWeapon;
}

bool AIngamePlayerState::Server_Reliable_SetStartingReserveWeapon_Validate(TSubclassOf<AInteractable> StartingReserveWeapon)
{ return true; }

void AIngamePlayerState::Server_Reliable_SetStartingReserveWeapon_Implementation(TSubclassOf<AInteractable> StartingReserveWeapon)
{
	_StartingReserveWeapon = StartingReserveWeapon;
}

bool AIngamePlayerState::Server_Reliable_SetStartingGrenadeType_Validate(E_GrenadeType StartingGrenadeType)
{ return true; }

void AIngamePlayerState::Server_Reliable_SetStartingGrenadeType_Implementation(E_GrenadeType StartingGrenadeType)
{
	_StartingGrenadeType = StartingGrenadeType;
}

bool AIngamePlayerState::Server_Reliable_SetStartingGrenadeCount_Validate(int StartingGrenadeCount)
{ return true; }

void AIngamePlayerState::Server_Reliable_SetStartingGrenadeCount_Implementation(int StartingGrenadeCount)
{
	_StartingGrenadeCount = StartingGrenadeCount;
}

bool AIngamePlayerState::Server_Reliable_SetStartingEquipment_Validate(E_EquipmentType StartingEquipment)
{ return true; }

void AIngamePlayerState::Server_Reliable_SetStartingEquipment_Implementation(E_EquipmentType StartingEquipment)
{
	_StartingEquipment = StartingEquipment;
}
