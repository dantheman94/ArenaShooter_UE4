// Fill out your copyright notice in the Description page of Project Settings.

#include "IngamePlayerController.h"

#include "Character/ArenaCharacter.h"
#include "Ammo.h"
#include "Weapon.h"
#include "FireMode.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Input |

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* @summary:	Called to bind functionality to input.
*
* @return:	virtual void
*/
void AIngamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Bind base controls
	InputComponent->BindAxis("LookUp", this, &AIngamePlayerController::LookUp);
	InputComponent->BindAxis("LookRight", this, &AIngamePlayerController::LookRight);
	InputComponent->BindAxis("MoveForward", this, &AIngamePlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AIngamePlayerController::MoveRight);

	// Bind movement controls
	InputComponent->BindAction("Dash", IE_Pressed, this, &AIngamePlayerController::DashEnter);
	InputComponent->BindAction("Grapple", IE_Pressed, this, &AIngamePlayerController::GrappleHookEnter);
	InputComponent->BindAction("Grapple", IE_Released, this, &AIngamePlayerController::GrappleHookExit);
	InputComponent->BindAction("Clamber", IE_Pressed, this, &AIngamePlayerController::Clamber);
	InputComponent->BindAction("CrouchHold", IE_Pressed, this, &AIngamePlayerController::CrouchHoldEnter);
	InputComponent->BindAction("CrouchHold", IE_Released, this, &AIngamePlayerController::CrouchHoldExit);
	InputComponent->BindAction("CrouchToggle", IE_Pressed, this, &AIngamePlayerController::CrouchToggle);
	InputComponent->BindAction("AimWeapon", IE_Pressed, this, &AIngamePlayerController::HoverEnter);
	InputComponent->BindAction("AimWeapon", IE_Released, this, &AIngamePlayerController::HoverExit);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AIngamePlayerController::JumpEnter);
	InputComponent->BindAction("Jump", IE_Released, this, &AIngamePlayerController::JumpExit);
	InputComponent->BindAction("SlideHold", IE_Pressed, this, &AIngamePlayerController::SlideHoldEnter);
	InputComponent->BindAction("SlideHold", IE_Released, this, &AIngamePlayerController::SlideHoldExit);
	InputComponent->BindAction("SlideToggle", IE_Pressed, this, &AIngamePlayerController::SlideToggle);
	InputComponent->BindAction("SprintHold", IE_Pressed, this, &AIngamePlayerController::SprintHoldEnter);
	InputComponent->BindAction("SprintHold", IE_Released, this, &AIngamePlayerController::SprintHoldExit);
	InputComponent->BindAction("SprintToggle", IE_Pressed, this, &AIngamePlayerController::SprintToggle);

	// Bind combat controls
	InputComponent->BindAction("AimWeapon", IE_Pressed, this, &AIngamePlayerController::AimWeaponEnter);
	InputComponent->BindAction("AimWeapon", IE_Released, this, &AIngamePlayerController::AimWeaponExit);
	InputComponent->BindAction("FirePrimary", IE_Pressed, this, &AIngamePlayerController::FirePrimaryWeaponEnter);
	InputComponent->BindAction("FirePrimary", IE_Released, this, &AIngamePlayerController::FirePrimaryWeaponExit);
	InputComponent->BindAction("FireSecondary", IE_Pressed, this, &AIngamePlayerController::FireSecondaryWeaponEnter);
	InputComponent->BindAction("FireSecondary", IE_Released, this, &AIngamePlayerController::FireSecondaryWeaponExit);
	InputComponent->BindAction("ReloadPrimary", IE_Pressed, this, &AIngamePlayerController::ReloadPrimaryWeapon);
	InputComponent->BindAction("ReloadSecondary", IE_Pressed, this, &AIngamePlayerController::ReloadSecondaryWeapon);
	InputComponent->BindAction("Grenade", IE_DoubleClick, this, &AIngamePlayerController::ChargeGrenade);
	InputComponent->BindAction("Grenade", IE_Released, this, &AIngamePlayerController::ThrowGrenade);
	InputComponent->BindAction("Melee", IE_Pressed, this, &AIngamePlayerController::Melee);
	InputComponent->BindAction("ToggleWeapon", IE_Pressed, this, &AIngamePlayerController::ToggleWeapon);

	// Bind interaction
	InputComponent->BindAction("Interact", IE_Pressed, this, &AIngamePlayerController::InteractPrimary);
	InputComponent->BindAction("Interact", IE_Released, this, &AIngamePlayerController::CancelInteractionPrimary);
	InputComponent->BindAction("DuelWield", IE_Pressed, this, &AIngamePlayerController::InteractSecondary);
	InputComponent->BindAction("DuelWield", IE_Released, this, &AIngamePlayerController::CancelInteractionSecondary);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Interaction 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AIngamePlayerController::InteractPrimary()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
	{
		pawn->SetIsTryingToInteractPrimary(true);
		pawn->InteractPrimary();
	}
}

void AIngamePlayerController::InteractSecondary()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
	{
		pawn->SetIsTryingToInteractSecondary(true);
		pawn->InteractSecondary();
	}
}

void AIngamePlayerController::CancelInteractionPrimary()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
	{
		pawn->SetIsTryingToInteractPrimary(false);
		pawn->CancelInteraction();
	}
}

void AIngamePlayerController::CancelInteractionSecondary()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
	{
		pawn->SetIsTryingToInteractSecondary(false);
		pawn->CancelInteraction();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Base 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* @summary:	Moves the character (if valid) connected to the controller on the vertical axis (forward/backward).
*
* @param: float value
*
* @return: void
*/
void AIngamePlayerController::MoveForward(float Value)
{
	// Input check
	_fRawVerticalMovementInput = Value;
	if (_fRawVerticalMovementInput != 0.0f)
	{
		// Pawn check
		auto pawn = Cast<ABaseCharacter>(this->GetPawn());
		if (pawn)
		{
			if (pawn != NULL)
				pawn->MoveForward(Value);
		}
	} else
	{
		// Pawn check
		auto pawn = Cast<ABaseCharacter>(this->GetPawn());
		if (pawn)
		{
			if (pawn != NULL)
			{
				// Used for animation
				if (pawn->GetForwardInputScale() != Value)
					pawn->SetForwardInputScale(Value);
			}
		}
	}
}

/**
* @summary:	Moves the character (if valid) connected to the controller on the horizontal axis (left/right).
*
* @param: float value
*
* @return: void
*/
void AIngamePlayerController::MoveRight(float Value)
{
	// Input check
	_fRawHorzontalMovementInput = Value;
	if (_fRawHorzontalMovementInput != 0.0f)
	{
		// Pawn check
		auto pawn = Cast<ABaseCharacter>(this->GetPawn());
		if (pawn)
		{
			if (pawn != NULL)
				pawn->MoveRight(Value);
		}
	} else
	{
		// Pawn check
		auto pawn = Cast<ABaseCharacter>(this->GetPawn());
		if (pawn)
		{
			if (pawn != NULL)
			{
				// Used for animation
				if (pawn->GetForwardInputScale() != Value)
					pawn->SetForwardInputScale(Value);
			}
		}
	}
}

/**
* @summary:	Moves the character's aiming direction that is connected to the controller on the vertical axis (up/down).
*
* @param: float value
*
* @return: void
*/
void AIngamePlayerController::LookUp(float Value)
{
	// Input check
	_fRawVerticalLookInput = Value;
	if (_fRawVerticalLookInput != 0.0f)
	{
		// Pawn check
		auto pawn = Cast<ABaseCharacter>(this->GetPawn());
		if (pawn)
		{
			// Calculate raw input against deadzone
			FVector2D zonedInput = CalcScaledRadialDeadZonedInput(_fRawHorzontalLookInput, _fRawVerticalLookInput);

			// Get initial sensitivity based on controller type
			float sensitivity = 1.0f;
			switch (_eCurrentControllerType)
			{
			case E_ControllerType::eCT_Keyboard:	sensitivity = _fSensitivityKeyboardVertical; break;
			case E_ControllerType::eCT_Xbox:		sensitivity = _fSensitivityXbox; break;
			case E_ControllerType::eCT_PlayStation: sensitivity = _fSensitivityPlaystation; break;
			default: break;
			}

			// Multiply input scale to sensitivity, based on pawns movement state
			if (pawn->IsSprinting())
				sensitivity *= pawn->GetSprintLookInputScale();
			else
			{
				if (pawn->IsAiming())
					sensitivity *= _fAimLookInputScale;
			}

			// Add look input to pitch
			///pawn->Multicast_LookUp(zonedInput.Y * sensitivity);
			pawn->AddControllerPitchInput(zonedInput.Y * sensitivity * GetWorld()->GetDeltaSeconds());
		}
	}
}

/**
* @summary:	Moves the character's aiming direction that is connected to the controller on the horizontal axis (left/right).
*
* @param: float value
*
* @return: void
*/
void AIngamePlayerController::LookRight(float Value)
{
	// Input check
	_fRawHorzontalLookInput = Value;
	if (_fRawHorzontalLookInput != 0.0f)
	{
		// Pawn check
		auto pawn = Cast<ABaseCharacter>(this->GetPawn());
		if (pawn)
		{
			// Calculate raw input against deadzone
			FVector2D zonedInput = CalcScaledRadialDeadZonedInput(_fRawHorzontalLookInput, _fRawVerticalLookInput);

			// Get initial sensitivity based on controller type
			float sensitivity = 1.0f;
			switch (_eCurrentControllerType)
			{
			case E_ControllerType::eCT_Keyboard:	sensitivity = _fSensitivityKeyboardVertical; break;
			case E_ControllerType::eCT_Xbox:		sensitivity = _fSensitivityXbox; break;
			case E_ControllerType::eCT_PlayStation: sensitivity = _fSensitivityPlaystation; break;
			default: break;
			}

			// Multiply input scale to sensitivity, based on pawns movement state
			if (pawn->IsSprinting())
				sensitivity *= pawn->GetSprintLookInputScale();
			else
			{
				if (pawn->IsAiming())
					sensitivity *= _fAimLookInputScale;
			}

			// Add look input to yaw
			///pawn->Multicast_LookRight(zonedInput.X * sensitivity);
			pawn->AddControllerYawInput(zonedInput.X * sensitivity * GetWorld()->GetDeltaSeconds());
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Ability 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
void AIngamePlayerController::DashEnter()
{
	auto pawn = Cast<AArenaCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputDash();
}

/*
*
*/
void AIngamePlayerController::GrappleHookEnter()
{
	auto pawn = Cast<AArenaCharacter>(this->GetPawn());
	if (pawn)
		pawn->GrappleHookEnter();
}

/*
*
*/
void AIngamePlayerController::GrappleHookExit()
{
	auto pawn = Cast<AArenaCharacter>(this->GetPawn());
	if (pawn)
		pawn->GrappleHookExit();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Clamber 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
void AIngamePlayerController::Clamber()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Crouch 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
void AIngamePlayerController::CrouchHoldEnter()
{
	// BaseCharacter crouch
	///auto basecharacter = Cast<ABaseCharacter>(this->GetPawn());
	///if (basecharacter) { basecharacter->EnterCrouch(); }
}

/*
*
*/
void AIngamePlayerController::CrouchHoldExit()
{
	// BaseCharacter crouch
	///auto basecharacter = Cast<ABaseCharacter>(this->GetPawn());
	///if (basecharacter) { basecharacter->ExitCrouch(); }
}

/*
*
*/
void AIngamePlayerController::CrouchToggle()
{
	// Pawn check
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputCrouchToggle(!pawn->IsCrouching());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Hover 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
void AIngamePlayerController::HoverEnter()
{
	auto pawn = Cast<AArenaCharacter>(this->GetPawn());
	if (pawn)
		pawn->HoverEnter();
}

/*
*
*/
void AIngamePlayerController::HoverExit()
{
	auto pawn = Cast<AArenaCharacter>(this->GetPawn());
	if (pawn)
		pawn->HoverExit();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Jump 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
void AIngamePlayerController::JumpEnter()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputJump();
}

/*
*
*/
void AIngamePlayerController::JumpExit()
{
	auto pawn = Cast<AArenaCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputJumpExit();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Slide 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AIngamePlayerController::SlideHoldEnter()
{
	// BaseCharacter slide enter
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputSlideEnter();
}

void AIngamePlayerController::SlideHoldExit()
{
	// BaseCharacter slide exit
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputSlideExit();
}

void AIngamePlayerController::SlideToggle()
{
	// BaseCharacter slide toggle
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputSlideToggle(!pawn->IsSliding());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Sprint 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AIngamePlayerController::SprintHoldEnter()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->InputSprintEnter(); }
}

void AIngamePlayerController::SprintHoldExit()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->InputSprintExit(); }
}

void AIngamePlayerController::SprintToggle()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->InputSprintToggle(!pawn->IsSprinting()); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Vault 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AIngamePlayerController::Vault()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->InputVault(); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Weapon | Aiming 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AIngamePlayerController::AimWeaponEnter()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputAimEnter();
}

void AIngamePlayerController::AimWeaponExit()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputAimExit();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Weapon | Firing 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AIngamePlayerController::FirePrimaryWeaponEnter()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputPrimaryFirePress();
}

void AIngamePlayerController::FirePrimaryWeaponExit()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputPrimaryFireRelease();
}

void AIngamePlayerController::FireSecondaryWeaponEnter()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputSecondaryFirePress();
}

void AIngamePlayerController::FireSecondaryWeaponExit()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputSecondaryFireRelease();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Weapon | Grenade 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AIngamePlayerController::ChargeGrenade()
{

}

void AIngamePlayerController::ThrowGrenade()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Weapon | Melee 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AIngamePlayerController::Melee()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Weapon | Reloading 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AIngamePlayerController::ReloadPrimaryWeapon()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputReloadPrimaryWeapon();
}

void AIngamePlayerController::ReloadSecondaryWeapon()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputReloadSecondaryWeapon();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Weapon | Tabbing 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AIngamePlayerController::ToggleWeapon()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn)
		pawn->InputToggleWeapon();
}
