// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Ammo.generated.h"

// *** DEFINITIONS

#define _MAX_BATTERY_AMMO	100
#define _MAX_MAGAZINE_AMMO	100
#define _MAX_RESERVE_AMMO	100
#define _MAX_DAMAGE			100

// *** ENUMERATORS

UENUM(BlueprintType)
enum class E_AmmoType : uint8
{
	eAT_Magazine UMETA(DisplayName = "Magazine"),
	eAT_Battery UMETA(DisplayName = "Battery")
};

// *** CLASSES

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ARENASHOOTER_API UAmmo : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Startup ********************************************************************************************************************************

	/**
	* @summary:	Sets default values for this component's properties.
	*
	* @return:	Constructor
	*/
	UAmmo();

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

	///////////////////////////////////////////////

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// Current Frame **************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		int _iBatteryAmmo = 0;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		int _iBatteryClips = 0;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		int _iMagazineAmmoCount = 0;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		int _iReserveAmmoCount = 0;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		bool _bRoundInChamber = true;

	/*
	*	A timer handle used for referencing the reloading.
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		FTimerHandle _fReloadHandle;

	// Properties *****************************************************************************************************************************

	/*
	*	The channel identifier used for this ammo component.
	*	NOTE: If multiple firemode child components use the same channel whilst
	*	being attached to the same actor, they'll share the same ammo pool.
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		int _iAmmoChannel = 0;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data Table")
		bool _bUseDataTable = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data Table",
		meta = (EditCondition = "_bUseDataTable"))
		FString _sDataTableLocation;

	// Ammo ***********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo")
		E_AmmoType _eAmmoType = E_AmmoType::eAT_Magazine;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo",
		meta = (EditCondition = "!_bUseDataTable"))
		int _iBatteryMisfireThreshold = 10;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo",
		meta = (EditCondition = "!_bUseDataTable"))
		int _iMaximumBatteryCapacity = _MAX_BATTERY_AMMO;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo",
		meta = (EditCondition = "!_bUseDataTable"))
		int _iMaxBatteryClips = 1;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo",
		meta = (EditCondition = "!_bUseDataTable"))
		int _iMaximumMagazineAmmo = _MAX_MAGAZINE_AMMO;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo",
		meta = (EditCondition = "!_bUseDataTable"))
		int _iMaximumReserveAmmo = _MAX_RESERVE_AMMO;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo",
		meta = (EditCondition = "!_bUseDataTable"))
		int _iStartingBatteryCapacity = 100;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo",
		meta = (EditCondition = "!_bUseDataTable"))
		int _iStartingBatteryClips = 1;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo",
		meta = (EditCondition = "!_bUseDataTable"))
		int _iStartingMagazineAmmo = _MAX_MAGAZINE_AMMO;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo",
		meta = (EditCondition = "!_bUseDataTable"))
		int _iStartingReserveAmmo = _MAX_RESERVE_AMMO;

	// Reload *********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Reload")
		bool _bReplaceMagazineOnReload = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		int _iShotsFiredBeforeReload = 0;

public:	

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Current Frame **************************************************************************************************************************

	/**
	* @summary:	Called every frame.
	*
	* @param:	float DeltaTime
	*
	* @return:	virtual void
	*/
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Ammo ***********************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetMagazineCount(int StartingMagazineCount);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetReserveCount(int StartingReserveCount);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetBatteryAmmo(int StartingBatteryAmmo);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_DeductAmmo(int DeductionAmount);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetRoundInChamber(bool InChamber);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_DetermineIfBulletShouldBeInChamber();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		int GetAmmoChannel() { return _iAmmoChannel; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		E_AmmoType GetAmmoType() { return _eAmmoType; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetStartingMagazineAmmo() { return _iStartingMagazineAmmo; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetMagazineAmmo() { return _iMagazineAmmoCount; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetMaximumMagazineAmmo() { return _iMaximumMagazineAmmo; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetStartingReserveAmmo() { return _iStartingReserveAmmo; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetReserveAmmo() { return _iReserveAmmoCount; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetMaximumReserveAmmo() { return _iMaximumReserveAmmo; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetStartingBatteryAmmo() { return _iStartingBatteryCapacity; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetBatteryAmmo() { return _iBatteryAmmo; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetBatteryCapacity() { return _iMaximumBatteryCapacity; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetBatteryMisfireThreshold() { return _iBatteryMisfireThreshold; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetTotalAmmo() { return _iMagazineAmmoCount + _iReserveAmmoCount; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		bool IsRoundInChamber() { return _bRoundInChamber; }

	// Reload *********************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		void DelayedReload(float DelayTime);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_Reload();

	///////////////////////////////////////////////

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetShotsFiredBeforeReload(int ShotsFired);

};
