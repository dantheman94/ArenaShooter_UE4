// Fill out your copyright notice in the Description page of Project Settings.

#include "Stamina.h"

// Startup **************************************************************

/**
* @summary:	Sets default values for this component's properties.
*
* @return:	Constructor
*/
UStamina::UStamina()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

}

/**
* @summary:	Called when the game starts or when spawned.
*
* @return:	virtual void
*/
void UStamina::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

// Current Frame ********************************************************

/**
* @summary:	Called every frame.
*
* @param:	float DeltaTime
*
* @return:	virtual void
*/
void UStamina::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Stop firing tick function from next frame onwards (Sanity check)
	if (!_bShouldBeTicking) { SetComponentTickEnabled(false); return; }

	switch (_eStaminaState)
	{
	case E_StaminaStateType::ePT_Additive:
	{
		// Have we capped our stamina?
		if (_fStamina >= _MAX_STAMINA)
		{
			// Clamp to maximum
			_fStamina = _MAX_STAMINA;

			// Stop ticking
			_bShouldBeTicking = false;
			SetComponentTickEnabled(_bShouldBeTicking);

			_fOnEndRechargingStamina.Broadcast();
		}

		// Regenerate stamina
		else { _fStamina += _fRegenerationRate * GetWorld()->GetDeltaSeconds(); }

		break;
	}
	case E_StaminaStateType::ePT_Subtractive:
	{
		// Have we run out of stamina?
		if (_fStamina <= _MIN_STAMINA)
		{
			// Clamp to minimum
			_fStamina = _MIN_STAMINA;

			// Start recharge delay
			FTimerDelegate rechargeDelegate;
			rechargeDelegate.BindUFunction(this, FName("StartRechargingStamina"));
			GetWorld()->GetTimerManager().SetTimer(_fRechargeDelayHandle, rechargeDelegate, 1.0f, false, _fRechargeDelayTime);

			// Stop ticking
			_bShouldBeTicking = false;
			SetComponentTickEnabled(_bShouldBeTicking);
		}

		// Drain stamina
		else { _fStamina -= _fDrainRate * GetWorld()->GetDeltaSeconds(); }

		break;
	}
	default: break;
	}
}

/*
*
*/
void UStamina::StartRechargingStamina()
{
	_eStaminaState = E_StaminaStateType::ePT_Additive;
	_bShouldBeTicking = true;
	SetComponentTickEnabled(_bShouldBeTicking);

	_fOnStartRechargingStamina.Broadcast(1.0f);
}

/*
*
*/
void UStamina::StartDrainingStamina(float DrainRate = 0.0f)
{
	if (DrainRate != 0.0f)
	{ 		
		_fDefaultDrainRate = _fDrainRate;
		_fDrainRate = DrainRate; 
	}

	_eStaminaState = E_StaminaStateType::ePT_Subtractive;
	_bShouldBeTicking = true;
	SetComponentTickEnabled(_bShouldBeTicking);
}

/*
*
*/
void UStamina::StopDrainingStamina()
{
	// Stop ticking
	_bShouldBeTicking = false;
	SetComponentTickEnabled(_bShouldBeTicking);
	DelayedRecharge();
}

/*
*
*/
void UStamina::DelayedRecharge()
{
	// Start recharge delay
	FTimerDelegate rechargeDelegate;
	rechargeDelegate.BindUFunction(this, FName("StartRechargingStamina"));
	GetWorld()->GetTimerManager().SetTimer(_fRechargeDelayHandle, rechargeDelegate, 1.0f, false, _fRechargeDelayTime);

	// Stop ticking
	_bShouldBeTicking = false;
	SetComponentTickEnabled(_bShouldBeTicking);
}