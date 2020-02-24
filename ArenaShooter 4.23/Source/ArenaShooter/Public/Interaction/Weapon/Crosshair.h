// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Crosshair.generated.h"

// *** EVENT DISPATCHERS / DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShowHitMarker);

// *** CLASSES

class AWeapon;

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API UCrosshair : public UUserWidget
{
	GENERATED_BODY()

protected:

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// Weapon *********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY()
		AWeapon* _Weapon = NULL;

	UPROPERTY(BlueprintAssignable)
		FShowHitMarker _HitMarkerDelegate;

public:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Weapon *********************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void SetWeaponReference(AWeapon* Weapon) { _Weapon = Weapon; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		AWeapon* GetWeaponReference() { return _Weapon; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		FShowHitMarker GetHitMarkerDelegate() { return _HitMarkerDelegate; }

};
