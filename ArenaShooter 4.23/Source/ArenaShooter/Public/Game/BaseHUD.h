// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDMainMenu.h"
#include "BaseHUD.generated.h"

// *** CLASSES

class ABaseCharacter;
class AInteractable;
class AWeapon;
class UCrosshair;
class UFireMode;
class UUserWidget;

UCLASS()
class ARENASHOOTER_API ABaseHUD : public AHUDMainMenu
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
		bool _bDisplayVisorOverlay = true;

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

	// Visor
	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | ArenaCharacter HUD")
		TSubclassOf<class UUserWidget> _HUD_VisorOverlay;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widgets | ArenaCharacter HUD")
		UUserWidget* _HUD_VisorOverlay_Instance = NULL;

	// Crosshairs
	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widgets | ArenaCharacter HUD")
		UCrosshair* _HUD_PrimaryWeapon_Crosshair = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widgets | ArenaCharacter HUD")
		UCrosshair* _HUD_SecondaryWeapon_Crosshair = NULL;

	// Interaction ****************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY()
		AInteractable* _FocusInteractable = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | ArenaCharacter HUD")
		TSubclassOf<class UUserWidget> _HUD_InteractionPrompt;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widgets | ArenaCharacter HUD")
		UUserWidget* _HUD_InteractionPromptInstance = NULL;

	// References ****************************************************************************************************************************

	UPROPERTY()
		ABaseCharacter* _BaseCharacter = NULL;

	UPROPERTY()
		AWeapon* _PrimaryWeapon = NULL;

	UPROPERTY()
		UFireMode* _PrimaryWeaponFireMode = NULL;

	UPROPERTY()
		AWeapon* _SecondaryWeapon = NULL;

	UPROPERTY()
		UFireMode* _SecondaryWeaponFireMode = NULL;

public:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	/*
	*
	*/
	virtual void DrawHUD() override;

	UFUNCTION(exec)
		void Debug_DisplayHUD(bool Draw);

	UFUNCTION(BlueprintCallable)
		void SetDisplayHud(bool Display);

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
		UCrosshair* GetPrimaryWeaponCrosshair() const { return _HUD_PrimaryWeapon_Crosshair; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintSetter, Category = "Widgets | ArenaCharacter HUD")
		void SetPrimaryCrosshair(UCrosshair* Crosshair) { _HUD_PrimaryWeapon_Crosshair = Crosshair; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintGetter, Category = "Widgets | ArenaCharacter HUD")
		UCrosshair* GetSecondaryWeaponCrosshair() const { return _HUD_SecondaryWeapon_Crosshair; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintSetter, Category = "Widgets | ArenaCharacter HUD")
		void SetSecondaryCrosshair(UCrosshair* Crosshair) { _HUD_SecondaryWeapon_Crosshair = Crosshair; }

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

	// Visor **********************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_VisorOverlay();

	// Interaction ****************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_InteractablePickupPrompt();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void SetNewFocusInteractable(AInteractable* NewInteractable) { _FocusInteractable = NewInteractable; }

	///////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
		void SetWidgetInteractable(AInteractable* NewInteractable);

	///////////////////////////////////////////////

	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;

};
