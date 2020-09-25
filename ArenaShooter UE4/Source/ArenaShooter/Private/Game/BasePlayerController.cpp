// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"

#include "BaseGameInstance.h"
#include "BasePlayerState.h"
#include "Character/ArenaCharacter.h"
#include "CoreUObject/Public/UObject/UObjectGlobals.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/InputSettings.h"
#include "Math/Vector.h"

#include "Ammo.h"
#include "Weapon.h"
#include "FireMode.h"

/**
* @summary:	Sets default values for this actor's properties.
*
* @return:	Constructor
*/
ABasePlayerController::ABasePlayerController()
{
	_eCurrentControllerType = _eDefaultControllerType;
	_ePreviousControllerType = _eCurrentControllerType;
}

/**
* @summary:	Called when the game starts or when spawned.
*
* @return:	virtual void
*/
void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you do not need it.
	PrimaryActorTick.bCanEverTick = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Input

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* @summary:	Called to bind functionality to input.
*
* @return:	virtual void
*/
void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Bind base controls
	InputComponent->BindAxis("LookUp", this, &ABasePlayerController::LookUp);
	InputComponent->BindAxis("LookRight", this, &ABasePlayerController::LookRight);
	InputComponent->BindAxis("MoveForward", this, &ABasePlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABasePlayerController::MoveRight);

	// Bind movement controls
	InputComponent->BindAction("Dash", IE_Pressed, this, &ABasePlayerController::DashEnter);
	InputComponent->BindAction("Grapple", IE_Pressed, this, &ABasePlayerController::GrappleHookEnter);
	InputComponent->BindAction("Grapple", IE_Released, this, &ABasePlayerController::GrappleHookExit);
	InputComponent->BindAction("Clamber", IE_Pressed, this, &ABasePlayerController::Clamber);
	InputComponent->BindAction("CrouchHold", IE_Pressed, this, &ABasePlayerController::CrouchHoldEnter);
	InputComponent->BindAction("CrouchHold", IE_Released, this, &ABasePlayerController::CrouchHoldExit);
	InputComponent->BindAction("CrouchToggle", IE_Pressed, this, &ABasePlayerController::CrouchToggle);
	InputComponent->BindAction("AimWeapon", IE_Pressed, this, &ABasePlayerController::HoverEnter);
	InputComponent->BindAction("AimWeapon", IE_Released, this, &ABasePlayerController::HoverExit);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ABasePlayerController::JumpEnter);
	InputComponent->BindAction("Jump", IE_Released, this, &ABasePlayerController::JumpExit);
	InputComponent->BindAction("SlideHold", IE_Pressed, this, &ABasePlayerController::SlideHoldEnter);
	InputComponent->BindAction("SlideHold", IE_Released, this, &ABasePlayerController::SlideHoldExit);
	InputComponent->BindAction("SlideToggle", IE_Pressed, this, &ABasePlayerController::SlideToggle);
	InputComponent->BindAction("SprintHold", IE_Pressed, this, &ABasePlayerController::SprintHoldEnter);
	InputComponent->BindAction("SprintHold", IE_Released, this, &ABasePlayerController::SprintHoldExit);
	InputComponent->BindAction("SprintToggle", IE_Pressed, this, &ABasePlayerController::SprintToggle);

	// Bind combat controls
	InputComponent->BindAction("AimWeapon", IE_Pressed, this, &ABasePlayerController::AimWeaponEnter);
	InputComponent->BindAction("AimWeapon", IE_Released, this, &ABasePlayerController::AimWeaponExit);
	InputComponent->BindAction("FirePrimary", IE_Pressed, this, &ABasePlayerController::FirePrimaryWeaponEnter);
	InputComponent->BindAction("FirePrimary", IE_Released, this, &ABasePlayerController::FirePrimaryWeaponExit);
	InputComponent->BindAction("FireSecondary", IE_Pressed, this, &ABasePlayerController::FireSecondaryWeaponEnter);
	InputComponent->BindAction("FireSecondary", IE_Released, this, &ABasePlayerController::FireSecondaryWeaponExit);
	InputComponent->BindAction("ReloadPrimary", IE_Pressed, this, &ABasePlayerController::ReloadPrimaryWeapon);
	InputComponent->BindAction("ReloadSecondary", IE_Pressed, this, &ABasePlayerController::ReloadSecondaryWeapon);
	InputComponent->BindAction("Grenade", IE_DoubleClick, this, &ABasePlayerController::ChargeGrenade);
	InputComponent->BindAction("Grenade", IE_Released, this, &ABasePlayerController::ThrowGrenade);
	InputComponent->BindAction("Melee", IE_Pressed, this, &ABasePlayerController::Melee);
	InputComponent->BindAction("ToggleWeapon", IE_Pressed, this, &ABasePlayerController::ToggleWeapon);

	// Bind interaction
	InputComponent->BindAction("Interact", IE_Pressed, this, &ABasePlayerController::InteractPrimary);
	InputComponent->BindAction("Interact", IE_Released, this, &ABasePlayerController::CancelInteraction);
	InputComponent->BindAction("DuelWield", IE_Pressed, this, &ABasePlayerController::InteractSecondary);
	InputComponent->BindAction("DuelWield", IE_Released, this, &ABasePlayerController::CancelInteraction);
}

/**
* @summary:	Calculates raw input against the deadzone by a scaled radial factor.
*
* @param:	float horizonalInput
* @param:	float verticalInput
*
* @return:	FVector2D
*/
FVector2D ABasePlayerController::CalcScaledRadialDeadZonedInput(float HorizonalInput, float VerticalInput)
{
	FVector2D rawInput = FVector2D(HorizonalInput, VerticalInput);
	float deadzone = 0.1f;

	if (rawInput.Size() < deadzone) { rawInput = FVector2D::ZeroVector; } else { rawInput = rawInput.GetSafeNormal() * ((rawInput.Size() - deadzone) / (1 - deadzone)); }

	return rawInput;
}

/**
* @summary:	Sets the axis mapping information into oldBinding from newBinding.
*
* @param: FInputAxisKeyMapping oldBinding
* @param: FInputAxisKeyMapping newBinding
*
* @return: static void
*/
void ABasePlayerController::UpdateAxisMapping(FInputAxisKeyMapping& OldBinding, FInputAxisKeyMapping& NewBinding)
{
	OldBinding.Key = NewBinding.Key;
	OldBinding.Scale = NewBinding.Scale;
}

/**
* @summary:	Sets the action mapping information into oldBinding from newBinding.
*
* @param: FInputActionKeyMapping oldBinding
* @param: FInputActionKeyMapping newBinding
*
* @return: static void
*/
void ABasePlayerController::UpdateActionMapping(FInputActionKeyMapping& OldBinding, FInputActionKeyMapping& NewBinding)
{
	OldBinding.Key = NewBinding.Key;
	OldBinding.bShift = NewBinding.bShift;
	OldBinding.bCtrl = NewBinding.bCtrl;
	OldBinding.bAlt = NewBinding.bAlt;
	OldBinding.bCmd = NewBinding.bCmd;
}

/*
* Resources used: http://www.recursiveblueprints.fun/controller-detection/
*/
bool ABasePlayerController::InputAxis(FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	bool ret = Super::InputAxis(Key, Delta, DeltaTime, NumSamples, bGamepad);

	// Determine controller type
	if (bGamepad)
	{ _eCurrentControllerType = E_ControllerType::eCT_Xbox; }
	else
	{ _eCurrentControllerType = E_ControllerType::eCT_Keyboard; }

	// New controller type?
	if (_eCurrentControllerType != _ePreviousControllerType)
	{
		_fOnControllerSwitch.Broadcast(_eCurrentControllerType);
		_ePreviousControllerType = _eCurrentControllerType;
		GEngine->AddOnScreenDebugMessage(25, 5.0f, FColor::Magenta, TEXT("Controller Type Switch"));
	}

	return ret;
}

/*
* Resources used: http://www.recursiveblueprints.fun/controller-detection/
*/
bool ABasePlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	bool ret = Super::InputKey(Key, EventType, AmountDepressed, bGamepad);

	// Determine controller type
	if (bGamepad)
	{ _eCurrentControllerType = E_ControllerType::eCT_Xbox; } 
	else
	{ _eCurrentControllerType = E_ControllerType::eCT_Keyboard; }

	// New controller type?
	if (_eCurrentControllerType != _ePreviousControllerType)
	{
		_fOnControllerSwitch.Broadcast(_eCurrentControllerType);
		_ePreviousControllerType = _eCurrentControllerType;
		GEngine->AddOnScreenDebugMessage(25, 5.0f, FColor::Magenta, TEXT("Controller Type Switch"));
	}

	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Combat | Aiming 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ABasePlayerController::AimWeaponEnter()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->AimWeaponEnter(); }
}

void ABasePlayerController::AimWeaponExit()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->AimWeaponExit(); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Combat | Tabbing 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ABasePlayerController::ToggleWeapon()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->InputToggleWeapon(); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Combat | Weapon 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ABasePlayerController::FirePrimaryWeaponEnter()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->InputPrimaryFirePress(); }
}

void ABasePlayerController::FirePrimaryWeaponExit()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->InputPrimaryFireRelease(); }
}

void ABasePlayerController::FireSecondaryWeaponEnter()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->InputSecondaryFirePress(); }
}

void ABasePlayerController::FireSecondaryWeaponExit()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->InputSecondaryFireRelease(); }
}

void ABasePlayerController::ReloadPrimaryWeapon()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->InputReloadPrimaryWeapon(); }
}

void ABasePlayerController::ReloadSecondaryWeapon()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->InputReloadSecondaryWeapon(); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Combat | Grenade 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ABasePlayerController::ChargeGrenade()
{

}

void ABasePlayerController::ThrowGrenade()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Combat | Melee 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ABasePlayerController::Melee()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Interaction 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ABasePlayerController::InteractPrimary()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->InteractPrimary(); }
}

void ABasePlayerController::InteractSecondary()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->InteractSecondary(); }
}

void ABasePlayerController::CancelInteraction()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->CancelInteraction(); }
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
void ABasePlayerController::MoveForward(float Value)
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
			{ pawn->MoveForward(Value); }
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
				if (pawn->GetForwardInputScale() != Value) { pawn->SetForwardInputScale(Value); }
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
void ABasePlayerController::MoveRight(float Value)
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
			{ pawn->MoveRight(Value); }
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
				if (pawn->GetForwardInputScale() != Value) { pawn->SetForwardInputScale(Value); }
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
void ABasePlayerController::LookUp(float Value)
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
			if (pawn->IsSprinting()) { sensitivity *= pawn->GetSprintLookInputScale(); } else
			{
				if (pawn->IsAiming()) { sensitivity *= _fAimLookInputScale; }
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
void ABasePlayerController::LookRight(float Value)
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
			if (pawn->IsSprinting()) { sensitivity *= pawn->GetSprintLookInputScale(); } 
			else
			{
				if (pawn->IsAiming()) { sensitivity *= _fAimLookInputScale; }
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
void ABasePlayerController::DashEnter()
{
	auto pawn = Cast<AArenaCharacter>(this->GetPawn());
	if (pawn) { pawn->InputDash(); }
}

/*
*
*/
void ABasePlayerController::GrappleHookEnter()
{
	auto pawn = Cast<AArenaCharacter>(this->GetPawn());
	if (pawn) { pawn->GrappleHookEnter(); }
}

/*
*
*/
void ABasePlayerController::GrappleHookExit()
{
	auto pawn = Cast<AArenaCharacter>(this->GetPawn());
	if (pawn) { pawn->GrappleHookExit(); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Clamber 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
void ABasePlayerController::Clamber()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Crouch 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
void ABasePlayerController::CrouchHoldEnter()
{
	// BaseCharacter crouch
	///auto basecharacter = Cast<ABaseCharacter>(this->GetPawn());
	///if (basecharacter) { basecharacter->EnterCrouch(); }
}

/*
*
*/
void ABasePlayerController::CrouchHoldExit()
{
	// BaseCharacter crouch
	///auto basecharacter = Cast<ABaseCharacter>(this->GetPawn());
	///if (basecharacter) { basecharacter->ExitCrouch(); }
}

/*
*
*/
void ABasePlayerController::CrouchToggle()
{
	// Pawn check
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->CrouchToggle(!pawn->IsCrouching()); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Hover 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
void ABasePlayerController::HoverEnter()
{
	auto pawn = Cast<AArenaCharacter>(this->GetPawn());
	if (pawn) { pawn->HoverEnter(); }
}

/*
*
*/
void ABasePlayerController::HoverExit()
{
	auto pawn = Cast<AArenaCharacter>(this->GetPawn());
	if (pawn) { pawn->HoverExit(); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Jump 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
void ABasePlayerController::JumpEnter()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->InputJump(); }
}

/*
*
*/
void ABasePlayerController::JumpExit()
{
	auto pawn = Cast<AArenaCharacter>(this->GetPawn());
	if (pawn) { pawn->InputJumpExit(); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Slide 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ABasePlayerController::SlideHoldEnter()
{
	// ArenaCharacter slide enter
	auto pawn = Cast<AArenaCharacter>(this->GetPawn());
	if (pawn) { pawn->InputSlideEnter(); }
}

void ABasePlayerController::SlideHoldExit()
{
	// ArenaCharacter slide exit
	auto pawn = Cast<AArenaCharacter>(this->GetPawn());
	if (pawn) { pawn->InputSlideExit(); }
}

void ABasePlayerController::SlideToggle()
{
	// ArenaCharacter slide toggle
	auto pawn = Cast<AArenaCharacter>(this->GetPawn());
	if (pawn) { pawn->InputSlideToggle(!pawn->IsSliding()); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Sprint 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ABasePlayerController::SprintHoldEnter()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->SprintEnter(); }
}

void ABasePlayerController::SprintHoldExit()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->SprintExit(); }
}

void ABasePlayerController::SprintToggle()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->SprintToggle(!pawn->IsSprinting()); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Vault 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ABasePlayerController::Vault()
{
	auto pawn = Cast<ABaseCharacter>(this->GetPawn());
	if (pawn) { pawn->InputVault(); }
}

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
void ABasePlayerController::Tick(float DeltaTime)
{
	ABasePlayerState* playerState = Cast<ABasePlayerState>(PlayerState);
	if (playerState != NULL)
	{
		if (playerState->GetPlayerInfo().GetPlayerController() == NULL)
		{
			playerState->SetPlayerController(this);
			///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Bepis"));
		}		
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Controller 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ABasePlayerController::GamepadRumble(float Intensity, float Duration,
	bool AffectsLeftLarge, bool AffectsLeftSmall, bool AffectsRightLarge, bool AffectsRightSmall)
{
	PlayDynamicForceFeedback(Intensity, Duration, AffectsLeftLarge, AffectsLeftSmall, AffectsRightLarge, AffectsRightSmall);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Input | Rebinding 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* @summary:	Tries to find the matching axis binding & replaces the input key with the new binding.
*
* @param: FInputAxisKeyMapping oldBinding
* @param: FInputAxisKeyMapping newBinding
*
* @return: boolean
*/
bool ABasePlayerController::RebindAxisKey(FInputAxisKeyMapping oldBinding, FInputAxisKeyMapping newBinding)
{
	// Get input settings
	const UInputSettings* inputSettings = GetDefault<UInputSettings>();
	if (!inputSettings) { return false; }

	// Get all current axis mappings
	TArray<struct FInputAxisKeyMapping> axi = inputSettings->GetAxisMappings();

	// Look for matching axis mapping
	bool matchFound = false;
	for (FInputAxisKeyMapping iter : axi)
	{
		if (matchFound = (iter.AxisName.ToString() == oldBinding.AxisName.ToString()))
		{
			UpdateAxisMapping(iter, newBinding);
			break;
		}
	}

	if (matchFound)
	{
		// Save to disk
		const_cast<UInputSettings*>(inputSettings)->SaveKeyMappings();

		// Rebuild config (Saved/Config/Windows/Input.ini)
		for (TObjectIterator<UPlayerInput> iter; iter; ++iter)
		{
			iter->ForceRebuildingKeyMaps(true);
		}
	}
	return matchFound;
}

/**
* @summary:	Tries to find the matching action binding & replaces the input key with the new binding.
*
* @param: FInputActionKeyMapping oldBinding
* @param: FInputActionKeyMapping newBinding
*
* @return: boolean
*/
bool ABasePlayerController::RebindActionKey(FInputActionKeyMapping oldBinding, FInputActionKeyMapping newBinding)
{
	// Get input settings
	const UInputSettings* inputSettings = GetDefault<UInputSettings>();
	if (!inputSettings) { return false; }

	// Get all current action mappings
	const TArray<FInputActionKeyMapping> actions = inputSettings->GetActionMappings();

	// Look for matching action mapping
	bool matchFound = false;
	for (FInputActionKeyMapping iter : actions)
	{
		if (matchFound = (iter.ActionName.ToString() == oldBinding.ActionName.ToString()))
		{
			UpdateActionMapping(iter, newBinding);
			break;
		}
	}

	if (matchFound)
	{
		// Save to disk
		const_cast<UInputSettings*>(inputSettings)->SaveKeyMappings();

		// Rebuild config (Saved/Config/Windows/Input.ini)
		for (TObjectIterator<UPlayerInput> iter; iter; ++iter)
		{
			iter->ForceRebuildingKeyMaps(true);
		}
	}
	return matchFound;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Debug 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ABasePlayerController::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool handled = Super::ProcessConsoleExec(Cmd, Ar, Executor);
	if (!handled)
	{
		handled &= this->ProcessConsoleExec(Cmd, Ar, Executor);
	}
	return handled;
}
