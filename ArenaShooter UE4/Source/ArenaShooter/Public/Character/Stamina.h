// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Stamina.generated.h"

#define _MAX_STAMINA	100
#define _MIN_STAMINA	0

UENUM(BlueprintType)
enum class E_StaminaStateType : uint8
{
	ePT_Additive UMETA(DisplayName = "Recharging"),
	ePT_Subtractive UMETA(DisplayName = "Draining")
};

// *** EVENT DISPATCHERS / DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartRechargingStamina, float, RechargeTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndRechargingStamina);

// *** CLASSES

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ARENASHOOTER_API UStamina : public UActorComponent
{
	GENERATED_BODY()

public:

	// Startup **************************************************************

	/**
	* @summary:	Sets default values for this component's properties.
	*
	* @return:	Constructor
	*/
	UStamina();

protected:

	// Startup **************************************************************

	// *** FUNCTIONS

	/**
	* @summary:	Called when the game starts or when spawned.
	*
	* @return:	virtual void
	*/
	virtual void BeginPlay() override;

	// *** VARIABLES

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		bool _bShouldBeTicking = false;

	/*
	*	The current state of the stamina channel (additive/subtractive)
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		E_StaminaStateType _eStaminaState = E_StaminaStateType::ePT_Subtractive;

	/*
	*	The current value of stamina in this channel.
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float _fStamina = _MAX_STAMINA;

	/*
	*	The channel identifier used for this stamina component.
	*	WARNING: If multiple stamina child components use the same channel whilst
	*	being attached to the same actor, undesirable results may happen.
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		int _iStaminaChannel = 0;

	/*
	*	The rate of drainage on the stamina when its in a subtractive state.
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		float _fDrainRate = 2.0f;

	/*
	*	The rate of drainage on the stamina when its in a subtractive state.
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
		float _fDefaultDrainRate = 2.0f;

	/*
	*	The rate of regeneration on the stamina when its in an additive state.
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		float _fRegenerationRate = 1.0f;

	/*
	*	The amount of time (in seconds) that is paused before the stamina begins to recharge.
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		float _fRechargeDelayTime = 1.5f;

	/*
	*	A timer handle used for referencing the recharge delay of the stamina channel.
	*/
	UPROPERTY()
		FTimerHandle _fRechargeDelayHandle;

	/*
	*
	*/
	UPROPERTY(BlueprintAssignable)
		FOnStartRechargingStamina _fOnStartRechargingStamina;

	/*
	*
	*/
	UPROPERTY(BlueprintAssignable)
		FOnEndRechargingStamina _fOnEndRechargingStamina;

public:

	// Current Frame ********************************************************

	// *** FUNCTIONS

	/**
	* @summary:	Called every frame.
	*
	* @param:	float DeltaTime
	*
	* @return:	virtual void
	*/
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void StartRechargingStamina();

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void StartDrainingStamina(float DrainRate);

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void StopDrainingStamina();

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		float GetStamina() { return _fStamina; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		float GetMaxStaminaAmount() { return _MAX_STAMINA; }

	/*
	*
	*/
	UFUNCTION()
		float GetRechargeDelayTime() { return _fRechargeDelayTime; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		bool IsFullyRecharged() { return _fStamina == _MAX_STAMINA; }

	/*
	*
	*/
	UFUNCTION()
		void SetStamina(float value) { _fStamina = value; }

	/*
	*
	*/
	UFUNCTION()
		int GetStaminaChannel() { return _iStaminaChannel; }

	/*
	*
	*/
	UFUNCTION()
		bool IsDraining() { return _bShouldBeTicking; }

	/*
	*
	*/
	UFUNCTION()
		bool HasStamina() { return _fStamina > 0.0f; }

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void ClearStamina() { SetStamina(0.0f); }

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void DelayedRecharge();

};