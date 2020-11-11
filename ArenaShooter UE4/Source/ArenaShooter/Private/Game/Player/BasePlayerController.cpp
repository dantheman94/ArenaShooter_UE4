// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"

#include "BaseGameInstance.h"
#include "BasePlayerState.h"
#include "CoreUObject/Public/UObject/UObjectGlobals.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/InputSettings.h"
#include "Math/Vector.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Startup

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

// Input

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
