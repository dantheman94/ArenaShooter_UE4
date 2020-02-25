// Fill out your copyright notice in the Description page of Project Settings.

#include "Ammo.h"

#include "UnrealNetwork.h"

// Sets default values for this component's properties
UAmmo::UAmmo()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Component replicates
	bReplicates = true;
}


// Called when the game starts
void UAmmo::BeginPlay()
{
	Super::BeginPlay();

	// Starting ammo
	Server_Reliable_SetMagazineCount(_iStartingMagazineAmmo);
	Server_Reliable_SetReserveCount(_iStartingReserveAmmo);
	Server_Reliable_SetBatteryAmmo(_iStartingBatteryCapacity);
}


void UAmmo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAmmo, _bRoundInChamber);
	DOREPLIFETIME(UAmmo, _iBatteryAmmo);
	DOREPLIFETIME(UAmmo, _iMagazineAmmoCount);
	DOREPLIFETIME(UAmmo, _iReserveAmmoCount);
	DOREPLIFETIME(UAmmo, _iShotsFiredBeforeReload);
}

// Current Frame **************************************************************************************************************************

// Called every frame
void UAmmo::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

///////////////////////////////////////////////

bool UAmmo::Server_Reliable_SetBatteryAmmo_Validate(int StartingBatteryAmmo)
{ return true; }

void UAmmo::Server_Reliable_SetBatteryAmmo_Implementation(int StartingBatteryAmmo)
{
	_iBatteryAmmo = StartingBatteryAmmo;
}

///////////////////////////////////////////////

bool UAmmo::Server_Reliable_SetMagazineCount_Validate(int StartingMagazineCount)
{ return true; }

void UAmmo::Server_Reliable_SetMagazineCount_Implementation(int StartingMagazineCount)
{
	_iMagazineAmmoCount = StartingMagazineCount;
}

///////////////////////////////////////////////

bool UAmmo::Server_Reliable_SetReserveCount_Validate(int StartingReserveCount)
{ return true; }

void UAmmo::Server_Reliable_SetReserveCount_Implementation(int StartingReserveCount)
{
	_iReserveAmmoCount = StartingReserveCount;
}

// Ammo ***********************************************************************************************************************************

///////////////////////////////////////////////

bool UAmmo::Server_Reliable_DeductAmmo_Validate(int DeductionAmount)
{ return true; }

void UAmmo::Server_Reliable_DeductAmmo_Implementation(int DeductionAmount)
{
	switch (_eAmmoType)
	{
	case E_AmmoType::eAT_Magazine:
	{
		// Deduct from magazine 
		_iMagazineAmmoCount -= DeductionAmount;

		// Clamp to zero
		if (_iMagazineAmmoCount < 0) { _iMagazineAmmoCount = 0; }

		break;
	}

	case E_AmmoType::eAT_Battery:
	{
		// Deduct from battery 
		_iBatteryAmmo -= DeductionAmount;

		// clamp to zero
		if (_iBatteryAmmo < 0) { _iBatteryAmmo = 0; }

		break;
	}
	default: break;
	}
}

///////////////////////////////////////////////

bool UAmmo::Server_Reliable_DetermineIfBulletShouldBeInChamber_Validate()
{ return true; }

void UAmmo::Server_Reliable_DetermineIfBulletShouldBeInChamber_Implementation()
{
	// Skip over if there is already a new chamber in the round (Ie: reloaded when the magazine still had ammo in it)
	if (_bRoundInChamber) { return; }

	// If there is at least 1 round left in the magazine, chamber it into the gun
	_bRoundInChamber = _iMagazineAmmoCount > 0;
}

///////////////////////////////////////////////

bool UAmmo::Server_Reliable_SetRoundInChamber_Validate(bool InChamber)
{ return true; }

void UAmmo::Server_Reliable_SetRoundInChamber_Implementation(bool InChamber)
{
	_bRoundInChamber = InChamber;
}

// Reload *********************************************************************************************************************************

/*
*
*/
void UAmmo::DelayedReload(float DelayTime)
{
	// Checks if whether there should be a new bullet in the chamber or not
	Server_Reliable_DetermineIfBulletShouldBeInChamber();

	// Start delay timer then reload
	FTimerDelegate reloadDelegate;
	reloadDelegate.BindUFunction(this, FName("Reload"));
	GetWorld()->GetTimerManager().SetTimer(_fReloadHandle, reloadDelegate, 1.0f, false, DelayTime);
}

///////////////////////////////////////////////

/*
*
*/
bool UAmmo::Server_Reliable_Reload_Validate()
{ return true; }

void UAmmo::Server_Reliable_Reload_Implementation()
{
	// Replace magazine entirely
	if (_bReplaceMagazineOnReload)
	{
		// Full magazine available
		if (_iReserveAmmoCount >= _iMagazineAmmoCount)
		{
			// Replenish magazine to full
			_iMagazineAmmoCount = _iMaximumMagazineAmmo;

			// Deduct magazine size from reserves
			_iReserveAmmoCount -= _iMaximumMagazineAmmo;
		}

		// Full magazine is NOT available, only partial
		else
		{
			// Fill mag with whatever is left
			_iMagazineAmmoCount = _iReserveAmmoCount;

			// Reserves are now empty
			_iReserveAmmoCount = 0;
		}
	} 
	
	// DONT replace the entire magazine
	else
	{
		// Magazine hasn't been emptied completely
		if (_iShotsFiredBeforeReload < _iMaximumMagazineAmmo)
		{
			// More reserves than a full magazine available (Full magazine available)
			if (_iReserveAmmoCount >= _iMaximumMagazineAmmo)
			{
				// Deduct from reserves the gap from current magazine available to full magazine size (shots fired since full magazine)
				///int gap = _iMaximumMagazineAmmo - _iMagazineAmmoCount;
				_iReserveAmmoCount -= _iShotsFiredBeforeReload;

				// Replenish magazine to full
				_iMagazineAmmoCount = _iMaximumMagazineAmmo;
			}

			// Full magazine is NOT available, only partial
			else
			{
				// Deduct from reserves the gap from current magazine available to full magazine size (shots fired since full magazine)
				///int gap = _iMaximumMagazineAmmo - _iMagazineAmmoCount;
				_iReserveAmmoCount -= _iShotsFiredBeforeReload;

				// Fill the magazine with what we can
				int takenFromReserve = _iReserveAmmoCount - _iShotsFiredBeforeReload;
				_iMagazineAmmoCount += takenFromReserve;
				_iMagazineAmmoCount = FMath::Clamp(_iMagazineAmmoCount, 0, _iMaximumMagazineAmmo);
			}
		}

		// Replace mag entirely
		else
		{
			// Full magazine available
			if (_iReserveAmmoCount >= _iMagazineAmmoCount)
			{
				// Replenish magazine to full
				_iMagazineAmmoCount = _iMaximumMagazineAmmo;

				// Deduct magazine size from reserves
				_iReserveAmmoCount -= _iMaximumMagazineAmmo;
			}

			// Full magazine is NOT available, only partial
			else
			{
				// Fill mag with whatever is left
				_iMagazineAmmoCount = _iReserveAmmoCount;

				// Reserves are now empty
				_iReserveAmmoCount = 0;
			}
		}
	}

	///	_bReloadComplete = true;
}

///////////////////////////////////////////////

bool UAmmo::Server_Reliable_SetShotsFiredBeforeReload_Validate(int ShotsFired)
{ return true; }

void UAmmo::Server_Reliable_SetShotsFiredBeforeReload_Implementation(int ShotsFired)
{
	_iShotsFiredBeforeReload = ShotsFired;
}
