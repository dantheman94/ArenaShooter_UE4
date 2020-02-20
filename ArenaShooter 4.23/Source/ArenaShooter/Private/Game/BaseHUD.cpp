// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseHUD.h"

#include "BaseCharacter.h"
#include "FireMode.h"
#include "UserWidget.h"
#include "Weapon.h"

/*
*
*/
void ABaseHUD::DrawHUD()
{
	// Draw crosshairs
	TickDraw_PrimaryWeaponCrosshair();
	TickDraw_SecondaryWeaponCrosshair();
	
	// Draw healthbars
	TickDraw_Healthbars();
	TickDraw_StaminaBar();

	// Draw weapon stats
	TickDraw_PrimaryWeaponStats();
	TickDraw_SecondaryWeaponStats();
	TickDraw_ReserveWeaponStats();
	TickDraw_GrenadeInventory();

	// Draw visor
	TickDraw_VisorOverlay();

	// Draw interaction prompt
	TickDraw_InteractablePickupPrompt();
}

// Crosshair ******************************************************************************************************************************

void ABaseHUD::TickDraw_PrimaryWeaponCrosshair()
{
	// Widget instance exists and is referenced (sanity check)
	if (_HUD_PrimaryWeapon_Crosshair != NULL)
	{
		if (_HUD_PrimaryWeapon_Crosshair->IsInViewport() && _bDisplayCrosshairPrimaryWeapon) { return; }
		else
		{
			// Add to viewport
			if (_bDisplayCrosshairPrimaryWeapon)
			{ _HUD_PrimaryWeapon_Crosshair->AddToViewport(); }

			// Remove from viewport
			else
			{ _HUD_PrimaryWeapon_Crosshair->RemoveFromViewport(); }
		}
	}

	// No widget instance assigned >> Create a new one and assign it 
	else
	{
		// Sanity check(s)
		APlayerController* playerController = GetOwningPlayerController();
		if (playerController == NULL) { return; }
		APawn* playerPawn = playerController->GetPawn();
		if (playerPawn == NULL) { return; }
		ABaseCharacter* baseCharacter = Cast<ABaseCharacter>(playerPawn);
		if (baseCharacter == NULL) { return; }
		AWeapon* primaryWeapon = baseCharacter->GetPointerPrimaryWeapon();
		if (primaryWeapon == NULL) { return; }
		UFireMode* fireMode = primaryWeapon->GetCurrentFireMode();
		if (fireMode == NULL) { return; }

		// Create crosshair of the current primary weapon's firemode
		fireMode->Client_Reliable_CreateAndAssignCrosshair();
	}
}

///////////////////////////////////////////////

/*
*
*/
void ABaseHUD::TickDraw_SecondaryWeaponCrosshair()
{

}

// Healthbars *****************************************************************************************************************************

/*
*
*/
void ABaseHUD::TickDraw_Healthbars()
{
	// Widget instance exists and is referenced (sanity check)
	if (_HUD_HealthBars_Instance != NULL)
	{
		if (_HUD_HealthBars_Instance->IsInViewport() && _bDisplayHealthBars) { return; }
		else
		{
			// Add to viewport
			if (_bDisplayHealthBars)
			{ _HUD_HealthBars_Instance->AddToViewport(); }

			// Remove from viewport
			else
			{ _HUD_HealthBars_Instance->RemoveFromViewport(); }
		}
	}

	// No widget instance assigned >> Create a new one and assign it 
	else
	{
		// Sanity check
		if (_HUD_HealthBars == NULL) { return; }

		// Create and assign new UMG widget
		UUserWidget* widget = CreateWidget<UUserWidget>(this->GetOwningPlayerController(), _HUD_HealthBars);
		if (widget != NULL)
		{
			widget->AddToViewport();
			_HUD_HealthBars_Instance = widget;
		}
	}
}

///////////////////////////////////////////////

void ABaseHUD::TickDraw_StaminaBar()
{
	// Widget instance exists and is referenced (sanity check)
	if (_HUD_StaminaBar_Instance != NULL)
	{
		if (_HUD_StaminaBar_Instance->IsInViewport() && _bDisplayStaminabar) { return; }
		else
		{
			// Add to viewport
			if (_bDisplayStaminabar)
			{ _HUD_StaminaBar_Instance->AddToViewport(); }

			// Remove from viewport
			else
			{ _HUD_StaminaBar_Instance->RemoveFromViewport(); }
		}
	}

	// No widget instance assigned >> Create a new one and assign it 
	else
	{
		// Sanity check
		if (_HUD_StaminaBar == NULL) { return; }

		// Create and assign new UMG widget
		UUserWidget* widget = CreateWidget<UUserWidget>(this->GetOwningPlayerController(), _HUD_StaminaBar);
		if (widget != NULL)
		{
			widget->AddToViewport();
			_HUD_StaminaBar_Instance = widget;
		}
	}
}

// Weapon Stats ***************************************************************************************************************************

/*
*
*/
void ABaseHUD::TickDraw_PrimaryWeaponStats()
{
	// Widget instance exists and is referenced (sanity check)
	if (_HUD_PrimaryWeaponStats_Instance != NULL)
	{
		if (_HUD_PrimaryWeaponStats_Instance->IsInViewport() && _bDisplayStatsPrimaryWeapon) { return; } else
		{
			// Add to viewport
			if (_bDisplayStatsPrimaryWeapon)
			{ _HUD_PrimaryWeaponStats_Instance->AddToViewport(); }

			// Remove from viewport
			else
			{ _HUD_PrimaryWeaponStats_Instance->RemoveFromViewport(); }
		}
	}

	// No widget instance assigned >> Create a new one and assign it 
	else
	{
		// Sanity check
		if (_HUD_PrimaryWeapon == NULL) { return; }

		// Create and assign new UMG widget
		UUserWidget* widget = CreateWidget<UUserWidget>(this->GetOwningPlayerController(), _HUD_PrimaryWeapon);
		if (widget != NULL)
		{
			widget->AddToViewport();
			_HUD_PrimaryWeaponStats_Instance = widget;
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void ABaseHUD::TickDraw_SecondaryWeaponStats()
{
	// Widget instance exists and is referenced (sanity check)
	if (_HUD_SecondaryWeaponStats_Instance != NULL)
	{
		if (_HUD_SecondaryWeaponStats_Instance->IsInViewport() && _bDisplayStatsSecondaryWeapon) { return; } 
		else
		{
			// Add to viewport
			if (_bDisplayStatsSecondaryWeapon)
			{ _HUD_SecondaryWeaponStats_Instance->AddToViewport(); }

			// Remove from viewport
			else
			{ _HUD_SecondaryWeaponStats_Instance->RemoveFromViewport(); }
		}
	}

	// No widget instance assigned >> Create a new one and assign it
	else
	{
		// Sanity check
		if (_HUD_SecondaryWeapon == NULL) { return; }

		// Create and assign new UMG widget
		UUserWidget* widget = CreateWidget<UUserWidget>(this->GetOwningPlayerController(), _HUD_SecondaryWeapon);
		if (widget != NULL)
		{
			widget->AddToViewport();
			_HUD_SecondaryWeaponStats_Instance = widget;
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void ABaseHUD::TickDraw_ReserveWeaponStats()
{
	// Widget instance exists and is referenced (sanity check)
	if (_HUD_ReserveWeaponStats_Instance != NULL)
	{
		if (_HUD_ReserveWeaponStats_Instance->IsInViewport() && _bDisplayStatsReserveWeapon) { return; } else
		{
			// Add to viewport
			if (_bDisplayStatsReserveWeapon)
			{ _HUD_ReserveWeaponStats_Instance->AddToViewport(); }

			// Remove from viewport
			else
			{ _HUD_ReserveWeaponStats_Instance->RemoveFromViewport(); }
		}
	}

	// No widget instance assigned >> Create a new one and assign it
	else
	{
		// Sanity check
		if (_HUD_ReserveWeapon == NULL) { return; }

		// Create and assign new UMG widget
		UUserWidget* widget = CreateWidget<UUserWidget>(this->GetOwningPlayerController(), _HUD_ReserveWeapon);
		if (widget != NULL)
		{
			widget->AddToViewport();
			_HUD_ReserveWeaponStats_Instance = widget;
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void ABaseHUD::TickDraw_GrenadeInventory()
{
	// Widget instance exists and is referenced (sanity check)
	if (_HUD_GrenadeInventory_Instance != NULL)
	{
		if (_HUD_GrenadeInventory_Instance->IsInViewport() && _bDisplayGrenadeInventory) { return; } else
		{
			// Add to viewport
			if (_bDisplayGrenadeInventory)
			{ _HUD_GrenadeInventory_Instance->AddToViewport(); }

			// Remove from viewport
			else
			{ _HUD_GrenadeInventory_Instance->RemoveFromViewport(); }
		}
	}

	// No widget instance assigned >> Create a new one and assign it
	else
	{
		// Sanity check
		if (_HUD_GrenadeInventory == NULL) { return; }

		// Create and assign new UMG widget
		UUserWidget* widget = CreateWidget<UUserWidget>(this->GetOwningPlayerController(), _HUD_GrenadeInventory);
		if (widget != NULL)
		{
			widget->AddToViewport();
			_HUD_GrenadeInventory_Instance = widget;
		}
	}
}

// Visor **********************************************************************************************************************************

void ABaseHUD::TickDraw_VisorOverlay()
{
	// Widget instance exists and is referenced (sanity check)
	if (_HUD_VisorOverlay_Instance != NULL)
	{
		if (_HUD_VisorOverlay_Instance->IsInViewport() && _bDisplayVisorOverlay) { return; } else
		{
			// Add to viewport
			if (_bDisplayVisorOverlay)
			{ _HUD_VisorOverlay_Instance->AddToViewport(); }

			// Remove from viewport
			else
			{ _HUD_VisorOverlay_Instance->RemoveFromViewport(); }
		}
	}

	// No widget instance assigned >> Create a new one and assign it
	else
	{
		// Sanity check
		if (_HUD_VisorOverlay == NULL) { return; }

		// Create and assign new UMG widget
		UUserWidget* widget = CreateWidget<UUserWidget>(this->GetOwningPlayerController(), _HUD_VisorOverlay);
		if (widget != NULL)
		{
			widget->AddToViewport();
			_HUD_VisorOverlay_Instance = widget;
		}
	}
}

///////////////////////////////////////////////

void ABaseHUD::TickDraw_InteractablePickupPrompt()
{
	// Widget instance exists and is referenced (sanity check)
	if (_HUD_InteractionPromptInstance != NULL)
	{
		if (_HUD_InteractionPromptInstance->IsInViewport() && _bDisplayHUD) { return; } else
		{
			// Add to viewport
			if (_bDisplayHUD)
			{ _HUD_InteractionPromptInstance->AddToViewport(); }

			// Remove from viewport
			else
			{ _HUD_InteractionPromptInstance->RemoveFromViewport(); }
		}
	}

	// No widget instance assigned >> Create a new one and assign it
	else
	{
		// Sanity check
		if (_HUD_InteractionPrompt == NULL) { return; }

		// Create and assign new UMG widget
		UUserWidget* widget = CreateWidget<UUserWidget>(this->GetOwningPlayerController(), _HUD_InteractionPrompt);
		if (widget != NULL)
		{
			widget->AddToViewport();
			_HUD_InteractionPromptInstance = widget;
		}
	}
}

///////////////////////////////////////////////

void ABaseHUD::SetWidgetInteractable_Implementation(AInteractable* NewInteractable)
{

}
