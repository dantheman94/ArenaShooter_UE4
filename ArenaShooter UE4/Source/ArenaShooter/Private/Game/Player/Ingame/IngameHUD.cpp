// Fill out your copyright notice in the Description page of Project Settings.

#include "IngameHUD.h"

#include "BaseCharacter.h"
#include "Crosshair.h"
#include "FireMode.h"
#include "UserWidget.h"
#include "Weapon.h"

void AIngameHUD::DrawHUD()
{
	// Draw crosshairs
	TickDraw_PrimaryWeaponCrosshair();
	TickDraw_SecondaryWeaponCrosshair();

	// Draw healthbars
	TickDraw_Healthbars();
	TickDraw_StaminaBar();

	// Draw radar
	TickDraw_Radar();

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

void AIngameHUD::Debug_DisplayHUD(bool Draw)
{
	// Set master
	ABaseHUD::Debug_DisplayHUD(Draw);

	// Set individual HUD elements
	_bDisplayHealthBars = _bDisplayHUD;
	_bDisplayStaminabar = _bDisplayHUD;
	_bDisplayRadar = _bDisplayHUD;
	_bDisplayStatsPrimaryWeapon = _bDisplayHUD;
	_bDisplayStatsSecondaryWeapon = _bDisplayHUD;
	_bDisplayStatsReserveWeapon = _bDisplayHUD;
	_bDisplayGrenadeInventory = _bDisplayHUD;
	_bDisplayVisorOverlay = _bDisplayHUD;
	_bDisplayCrosshairPrimaryWeapon = _bDisplayHUD;
	_bDisplayCrosshairSecondaryWeapon = _bDisplayHUD;
}

void AIngameHUD::SetDisplayHud(bool Display)
{
	// Set master
	ABaseHUD::SetDisplayHud(Display);

	// Set individual HUD elements
	_bDisplayHealthBars = _bDisplayHUD;
	_bDisplayStaminabar = _bDisplayHUD;
	_bDisplayRadar = _bDisplayHUD;
	_bDisplayStatsPrimaryWeapon = _bDisplayHUD;
	_bDisplayStatsSecondaryWeapon = _bDisplayHUD;
	_bDisplayStatsReserveWeapon = _bDisplayHUD;
	_bDisplayGrenadeInventory = _bDisplayHUD;
	_bDisplayVisorOverlay = _bDisplayHUD;
	_bDisplayCrosshairPrimaryWeapon = _bDisplayHUD;
	_bDisplayCrosshairSecondaryWeapon = _bDisplayHUD;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Crosshair 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AIngameHUD::TickDraw_PrimaryWeaponCrosshair()
{
	// Widget instance exists and is referenced (sanity check)
	if (_HUD_PrimaryWeapon_Crosshair != NULL)
	{
		if (_HUD_PrimaryWeapon_Crosshair->IsInViewport() && _bDisplayCrosshairPrimaryWeapon) { /*return;*/ } else
		{
			// Add to viewport
			if (_bDisplayCrosshairPrimaryWeapon)
			{ _HUD_PrimaryWeapon_Crosshair->AddToViewport(); }

			// Remove from viewport 
			else
			{ _HUD_PrimaryWeapon_Crosshair->RemoveFromViewport(); }

			return;
		}

		// Double check that the crosshair widget class matches the correct primary weapon >> firemode
		if (_BaseCharacter == NULL) { _BaseCharacter = Cast<ABaseCharacter>(GetOwningPawn()); }
		if (_BaseCharacter == NULL) { return; }
		_PrimaryWeapon = _BaseCharacter->GetPointerPrimaryWeapon();
		if (_PrimaryWeapon == NULL && _HUD_PrimaryWeapon_Crosshair != NULL) { _HUD_PrimaryWeapon_Crosshair->RemoveFromViewport(); return; }
		if (_PrimaryWeapon == NULL) { return; }
		_PrimaryWeaponFireMode = _PrimaryWeapon->GetCurrentFireMode();
		if (_PrimaryWeaponFireMode == NULL && _HUD_PrimaryWeapon_Crosshair != NULL) { _HUD_PrimaryWeapon_Crosshair->RemoveFromViewport(); return; }
		if (_PrimaryWeaponFireMode == NULL) { return; }
		if (_HUD_PrimaryWeapon_Crosshair->GetClass() != _PrimaryWeaponFireMode->GetCrosshairClass())
		{
			// Crosshair UMG classes DONT match
			_HUD_PrimaryWeapon_Crosshair->SetWeaponReference(NULL);

			// Create and assign new UMG widget
			UCrosshair* widget = CreateWidget<UCrosshair>(this->GetOwningPlayerController(), _PrimaryWeaponFireMode->GetCrosshairClass());
			if (widget != NULL)
			{
				widget->AddToViewport();
				_HUD_PrimaryWeapon_Crosshair = widget;
				_HUD_PrimaryWeapon_Crosshair->SetWeaponReference(_PrimaryWeapon);
			}
		}
	}

	// No widget instance assigned >> Create a new one and assign it 
	else
	{
		// Sanity check(s)
		if (_BaseCharacter == NULL) { _BaseCharacter = Cast<ABaseCharacter>(GetOwningPawn()); }
		if (_BaseCharacter == NULL) { return; }
		_PrimaryWeapon = _BaseCharacter->GetPointerPrimaryWeapon();
		if (_PrimaryWeapon == NULL && _HUD_PrimaryWeapon_Crosshair != NULL) { _HUD_PrimaryWeapon_Crosshair->RemoveFromViewport(); return; }
		if (_PrimaryWeapon == NULL) { return; }
		_PrimaryWeaponFireMode = _PrimaryWeapon->GetCurrentFireMode();
		if (_PrimaryWeaponFireMode == NULL && _HUD_PrimaryWeapon_Crosshair != NULL) { _HUD_PrimaryWeapon_Crosshair->RemoveFromViewport(); return; }
		if (_PrimaryWeaponFireMode == NULL) { return; }

		// Create crosshair of the current primary weapon's firemode
		UCrosshair* widget = CreateWidget<UCrosshair>(this->GetOwningPlayerController(), _PrimaryWeaponFireMode->GetCrosshairClass());
		if (widget != NULL)
		{
			widget->AddToViewport();
			_HUD_PrimaryWeapon_Crosshair = widget;
			_HUD_PrimaryWeapon_Crosshair->SetWeaponReference(_PrimaryWeapon);
		}
	}
}

/*
*
*/
void AIngameHUD::TickDraw_SecondaryWeaponCrosshair()
{
	// Widget instance exists and is referenced (sanity check)
	if (_HUD_SecondaryWeapon_Crosshair != NULL)
	{
		if (_HUD_SecondaryWeapon_Crosshair->IsInViewport() && _bDisplayCrosshairSecondaryWeapon) { /*return;*/ } else
		{
			// Add to viewport
			if (_bDisplayCrosshairSecondaryWeapon)
			{ _HUD_SecondaryWeapon_Crosshair->AddToViewport(); }

			// Remove from viewport 
			else
			{ _HUD_SecondaryWeapon_Crosshair->RemoveFromViewport(); }

			return;
		}

		// Double check that the crosshair widget class matches the correct secondary weapon >> firemode
		if (_BaseCharacter == NULL) { _BaseCharacter = Cast<ABaseCharacter>(GetOwningPawn()); }
		if (_BaseCharacter == NULL) { return; }
		_SecondaryWeapon = _BaseCharacter->GetPointerSecondaryWeapon();
		if (_SecondaryWeapon == NULL && _HUD_SecondaryWeapon_Crosshair != NULL) { _HUD_SecondaryWeapon_Crosshair->RemoveFromViewport(); return; }
		if (_SecondaryWeapon == NULL) { return; }
		_SecondaryWeaponFireMode = _SecondaryWeapon->GetCurrentFireMode();
		if (_SecondaryWeaponFireMode == NULL && _HUD_SecondaryWeapon_Crosshair != NULL) { _HUD_SecondaryWeapon_Crosshair->RemoveFromViewport(); return; }
		if (_SecondaryWeaponFireMode == NULL) { return; }
		if (_HUD_SecondaryWeapon_Crosshair->GetClass() != _SecondaryWeaponFireMode->GetCrosshairClass())
		{
			// Crosshair UMG classes DONT match
			_HUD_SecondaryWeapon_Crosshair->SetWeaponReference(NULL);

			// Create and assign new UMG widget
			UCrosshair* widget = CreateWidget<UCrosshair>(this->GetOwningPlayerController(), _SecondaryWeaponFireMode->GetCrosshairClass());
			if (widget != NULL)
			{
				widget->AddToViewport();
				_HUD_SecondaryWeapon_Crosshair = widget;
				_HUD_SecondaryWeapon_Crosshair->SetWeaponReference(_SecondaryWeapon);
			}
		}
	}

	// No widget instance assigned >> Create a new one and assign it 
	else
	{
		// Sanity check(s)
		if (_BaseCharacter == NULL) { _BaseCharacter = Cast<ABaseCharacter>(GetOwningPawn()); }
		if (_BaseCharacter == NULL) { return; }
		_SecondaryWeapon = _BaseCharacter->GetPointerSecondaryWeapon();
		if (_SecondaryWeapon == NULL && _HUD_SecondaryWeapon_Crosshair != NULL) { _HUD_SecondaryWeapon_Crosshair->RemoveFromViewport(); return; }
		if (_SecondaryWeapon == NULL) { return; }
		_SecondaryWeaponFireMode = _SecondaryWeapon->GetCurrentFireMode();
		if (_SecondaryWeaponFireMode == NULL && _HUD_SecondaryWeapon_Crosshair != NULL) { _HUD_SecondaryWeapon_Crosshair->RemoveFromViewport(); return; }
		if (_SecondaryWeaponFireMode == NULL) { return; }

		// Create crosshair of the current primary weapon's firemode
		UCrosshair* widget = CreateWidget<UCrosshair>(this->GetOwningPlayerController(), _SecondaryWeaponFireMode->GetCrosshairClass());
		if (widget != NULL)
		{
			widget->AddToViewport();
			_HUD_SecondaryWeapon_Crosshair = widget;
			_HUD_SecondaryWeapon_Crosshair->SetWeaponReference(_SecondaryWeapon);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Healthbar 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
void AIngameHUD::TickDraw_Healthbars()
{
	// Widget instance exists and is referenced (sanity check)
	if (_HUD_HealthBars_Instance != NULL)
	{
		if (_HUD_HealthBars_Instance->IsInViewport() && _bDisplayHealthBars) { return; } else
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

void AIngameHUD::TickDraw_StaminaBar()
{
	// Widget instance exists and is referenced (sanity check)
	if (_HUD_StaminaBar_Instance != NULL)
	{
		if (_HUD_StaminaBar_Instance->IsInViewport() && _bDisplayStaminabar) { return; } else
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Radar 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
void AIngameHUD::TickDraw_Radar()
{
	// Widget instance exists and is referenced (sanity check)
	if (_HUD_Radar_Instance != NULL)
	{
		if (_HUD_Radar_Instance->IsInViewport() && _bDisplayRadar) { return; } else
		{
			// Add to viewport
			if (_bDisplayRadar)
			{ _HUD_Radar_Instance->AddToViewport(); }

			// Remove from viewport
			else
			{ _HUD_Radar_Instance->RemoveFromViewport(); }
		}
	}

	// No widget instance assigned >> Create a new one and assign it 
	else
	{
		// Sanity check
		if (_HUD_Radar == NULL) { return; }

		// Create and assign new UMG widget
		UUserWidget* widget = CreateWidget<UUserWidget>(this->GetOwningPlayerController(), _HUD_Radar);
		if (widget != NULL)
		{
			widget->AddToViewport();
			_HUD_Radar_Instance = widget;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Weapon Stats 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
void AIngameHUD::TickDraw_PrimaryWeaponStats()
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

/*
*
*/
void AIngameHUD::TickDraw_SecondaryWeaponStats()
{
	// Widget instance exists and is referenced (sanity check)
	if (_HUD_SecondaryWeaponStats_Instance != NULL)
	{
		if (_HUD_SecondaryWeaponStats_Instance->IsInViewport() && _bDisplayStatsSecondaryWeapon) { return; } else
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

/*
*
*/
void AIngameHUD::TickDraw_ReserveWeaponStats()
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

/*
*
*/
void AIngameHUD::TickDraw_GrenadeInventory()
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Visor 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AIngameHUD::TickDraw_VisorOverlay()
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

void AIngameHUD::TickDraw_InteractablePickupPrompt()
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

void AIngameHUD::SetWidgetInteractable_Implementation(UInteractionDataComponent* NewInteractable) {}
