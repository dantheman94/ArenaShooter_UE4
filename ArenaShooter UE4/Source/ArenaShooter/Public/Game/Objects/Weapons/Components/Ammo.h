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

#pragma region Protected Variables

protected:

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Current Frame 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Properties 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Ammo 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Reload

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

#pragma endregion Protected Variables

#pragma region Public Functions

public:	

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Startup

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @summary:	Sets default values for this component's properties.
	*
	* @return:	Constructor
	*/
	UAmmo();

	/**
	* @summary:	Called when the game starts or when spawned.
	*
	* @return:	virtual void
	*/
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Current Frame 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @summary:	Called every frame.
	*
	* @param:	float DeltaTime
	*
	* @return:	virtual void
	*/
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Ammo 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetMagazineCount(int Value);
	void SetMagazineCount(int Value);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetReserveCount(int Value);
	void SetReserveCount(int Value);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetBatteryAmmo(int Value);
	void SetBatteryAmmo(int Value);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_DeductAmmo(int DeductionAmount);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetRoundInChamber(bool InChamber);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_DetermineIfBulletShouldBeInChamber();

	/*
	*
	*/
	UFUNCTION() int GetAmmoChannel() { return _iAmmoChannel; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) E_AmmoType GetAmmoType() const { return _eAmmoType; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) int GetStartingMagazineAmmo() const { return _iStartingMagazineAmmo; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) int GetMagazineAmmo() const { return _iMagazineAmmoCount; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) int GetMaximumMagazineAmmo() const { return _iMaximumMagazineAmmo; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) int GetStartingReserveAmmo() const { return _iStartingReserveAmmo; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) int GetReserveAmmo() const { return _iReserveAmmoCount; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) int GetMaximumReserveAmmo() const { return _iMaximumReserveAmmo; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) int GetStartingBatteryAmmo() const { return _iStartingBatteryCapacity; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) int GetBatteryAmmo() const { return _iBatteryAmmo; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) int GetBatteryCapacity() const { return _iMaximumBatteryCapacity; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) int GetBatteryMisfireThreshold() const { return _iBatteryMisfireThreshold; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) int GetTotalAmmo() const { return _iMagazineAmmoCount + _iReserveAmmoCount; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) bool IsRoundInChamber() const { return _bRoundInChamber; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Reload

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void DelayedReload(float DelayTime);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_Reload();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetShotsFiredBeforeReload(int ShotsFired);

#pragma endregion Public Functions

};
