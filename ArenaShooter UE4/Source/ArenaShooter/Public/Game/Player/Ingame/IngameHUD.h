// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Player/BaseHUD.h"

#include "IngameHUD.generated.h"

class ABaseCharacter;
class AInteractable;
class AWeapon;
class UCrosshair;
class UFireMode;
class UUserWidget;

UCLASS()
class AIngameHUD : public ABaseHUD
{
	GENERATED_BODY()

#pragma region Protected Variables

protected:

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Display 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets | Display")
		bool _bDisplayHealthBars = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets | Display")
		bool _bDisplayStaminabar = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets | Display")
		bool _bDisplayRadar = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets | Display")
		bool _bDisplayStatsPrimaryWeapon = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets | Display")
		bool _bDisplayStatsSecondaryWeapon = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets | Display")
		bool _bDisplayStatsReserveWeapon = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets | Display")
		bool _bDisplayGrenadeInventory = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets | Display")
		bool _bDisplayVisorOverlay = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets | Display")
		bool _bDisplayCrosshairPrimaryWeapon = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets | Display")
		bool _bDisplayCrosshairSecondaryWeapon = true;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Healthbar 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Radar 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | ArenaCharacter HUD")
		TSubclassOf<class UUserWidget> _HUD_Radar;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Widgets | ArenaCharacter HUD")
		UUserWidget* _HUD_Radar_Instance = NULL;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Weapon Stats 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Visor 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Crosshair 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Interaction 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY()
		UInteractionDataComponent* _FocusInteractable = NULL;

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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// References 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

#pragma endregion Public Variables

#pragma region Public Functions

public:

	/*
	*
	*/
	virtual void DrawHUD() override;

	virtual void Debug_DisplayHUD(bool Draw) override;

	virtual void SetDisplayHud(bool Display) override;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Crosshair 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_PrimaryWeaponCrosshair();

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_SecondaryWeaponCrosshair();

	/*
	*
	*/
	UFUNCTION(BlueprintGetter, Category = "Ingame")
		UCrosshair* GetPrimaryWeaponCrosshair() const { return _HUD_PrimaryWeapon_Crosshair; }

	/*
	*
	*/
	UFUNCTION(BlueprintSetter, Category = "Ingame")
		void SetPrimaryCrosshair(UCrosshair* Crosshair) { _HUD_PrimaryWeapon_Crosshair = Crosshair; }

	/*
	*
	*/
	UFUNCTION(BlueprintGetter, Category = "Ingame")
		UCrosshair* GetSecondaryWeaponCrosshair() const { return _HUD_SecondaryWeapon_Crosshair; }

	/*
	*
	*/
	UFUNCTION(BlueprintSetter, Category = "Ingame")
		void SetSecondaryCrosshair(UCrosshair* Crosshair) { _HUD_SecondaryWeapon_Crosshair = Crosshair; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Healthbar 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_Healthbars();

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_StaminaBar();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Radar 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_Radar();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Weapon Stats 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_PrimaryWeaponStats();

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_SecondaryWeaponStats();

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_ReserveWeaponStats();

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_GrenadeInventory();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Visor 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_VisorOverlay();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Interaction 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void TickDraw_InteractablePickupPrompt();

	/*
	*
	*/
	UFUNCTION()
		void SetNewFocusInteractable(UInteractionDataComponent* NewInteractable) { _FocusInteractable = NewInteractable; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
		void SetWidgetInteractable(UInteractionDataComponent* NewInteractable);

#pragma endregion Public Functions

};