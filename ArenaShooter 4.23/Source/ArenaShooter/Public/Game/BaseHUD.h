// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseHUD.generated.h"

// *** CLASSES

class UUserWidget;

UCLASS()
class ARENASHOOTER_API ABaseHUD : public AHUD
{
	GENERATED_BODY()

protected:

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// Display ********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Display")
		bool _bDisplayHUD = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Display")
		bool _bDisplayHealthBars = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Display")
		bool _bDisplayStaminabar = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Display")
		bool _bDisplayStatsPrimaryWeapon = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Display")
		bool _bDisplayStatsSecondaryWeapon = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Display")
		bool _bDisplayStatsReserveWeapon = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Display")
		bool _bDisplayGrenadeInventory = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Display")
		bool _bDisplayCrosshairPrimaryWeapon = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Display")
		bool _bDisplayCrosshairSecondaryWeapon = true;

	// ArenaCharacter HUD *********************************************************************************************************************

	// Healthbars
	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | ArenaCharacter HUD")
		TSubclassOf<class UUserWidget> _HUD_HealthBars;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widgets | ArenaCharacter HUD")
		UUserWidget* _HUD_HealthBars_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | ArenaCharacter HUD")
		TSubclassOf<class UUserWidget> _HUD_StaminaBar;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widgets | ArenaCharacter HUD")
		UUserWidget* _HUD_StaminaBar_Instance = NULL;

	// Weapon Stats
	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | ArenaCharacter HUD")
		TSubclassOf<class UUserWidget> _HUD_PrimaryWeapon;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widgets | ArenaCharacter HUD")
		UUserWidget* _HUD_PrimaryWeaponStats_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | ArenaCharacter HUD")
		TSubclassOf<class UUserWidget> _HUD_SecondaryWeapon;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widgets | ArenaCharacter HUD")
		UUserWidget* _HUD_SecondaryWeaponStats_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | ArenaCharacter HUD")
		TSubclassOf<class UUserWidget> _HUD_ReserveWeapon;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widgets | ArenaCharacter HUD")
		UUserWidget* _HUD_ReserveWeaponStats_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | ArenaCharacter HUD")
		TSubclassOf<class UUserWidget> _HUD_GrenadeInventory;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widgets | ArenaCharacter HUD")
		UUserWidget* _HUD_GrenadeInventory_Instance = NULL;

	// Crosshairs
	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widgets | ArenaCharacter HUD")
		UUserWidget* _HUD_PrimaryWeapon_Crosshair = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widgets | ArenaCharacter HUD")
		UUserWidget* _HUD_SecondaryWeapon_Crosshair = NULL;

public:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	/*
	*
	*/
	virtual void DrawHUD() override;

	// Crosshair ******************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_PrimaryWeaponCrosshair();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_SecondaryWeaponCrosshair();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintGetter, Category = "Widgets | ArenaCharacter HUD")
		UUserWidget* GetPrimaryWeaponCrosshair() { return _HUD_PrimaryWeapon_Crosshair; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintSetter, Category = "Widgets | ArenaCharacter HUD")
		void SetPrimaryCrosshair(UUserWidget* Crosshair) { _HUD_PrimaryWeapon_Crosshair = Crosshair; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintGetter, Category = "Widgets | ArenaCharacter HUD")
		UUserWidget* GetSecondaryWeaponCrosshair() { return _HUD_SecondaryWeapon_Crosshair; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintSetter, Category = "Widgets | ArenaCharacter HUD")
		void SetSecondaryCrosshair(UUserWidget* Crosshair) { _HUD_SecondaryWeapon_Crosshair = Crosshair; }

	// Healthbars *****************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_Healthbars();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_StaminaBar();

	// Weapon Stats ***************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_PrimaryWeaponStats();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_SecondaryWeaponStats();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_ReserveWeaponStats();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_GrenadeInventory();

};
