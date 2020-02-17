// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/InputSettings.h"
#include "BasePlayerController.generated.h"

// *** ENUMERATORS

UENUM(BlueprintType)
enum class E_ControllerType : uint8
{
	eCT_Keyboard UMETA(DisplayName = "Keyboard"),
	eCT_Xbox UMETA(DisplayName = "Xbox Gamepad"),
	eCT_PlayStation UMETA(DisplayName = "PlayStation  Gamepad")
};

// *** CLASSES

struct FInputActionKeyMapping;
struct FInputAxisKeyMapping;

UCLASS()
class ARENASHOOTER_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	ABasePlayerController();

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

	// Combat | Aiming ******************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combat | Aiming")
		float _fDefaultFov = 90.0f;

	// Input | Sensitivity **************************************************

	float _fRawVerticalMovementInput = 0.0f;
	float _fRawHorzontalMovementInput = 0.0f;
	float _fRawVerticalLookInput = 0.0f;
	float _fRawHorzontalLookInput = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Input | Sensitivity")
		E_ControllerType _eControllerType = E_ControllerType::eCT_Keyboard;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input | Sensitivity")
		float _fSensitivityKeyboardVertical = 20.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input | Sensitivity")
		float _fSensitivityDesktopHorizontal = 20.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input | Sensitivity")
		float _fSensitivityXbox = 50.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input | Sensitivity")
		float _fSensitivityPlaystation = 50.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input | Sensitivity")
		float _fAimLookInputScale = 0.6f;

	// Input | Double Click *************************************************

	UPROPERTY(BlueprintReadWrite, Category = "Input | Double Click")
		float _fDoubleClickTime = 0.2f;

	// *** FUNCTIONS

	// Input ****************************************************************

	/**
	* @summary:	Called to bind functionality to input.
	*
	* @param:	UInputComponent* PlayerInputComponent
	*
	* @return:	virtual void
	*/
	virtual void SetupInputComponent() override;

	/**
	* @summary:	Calculates raw input against the deadzone by a scaled radial factor.
	*
	* @param:	float horizonalInput
	* @param:	float verticalInput
	*
	* @return:	Vector2D
	*/
	FVector2D CalcScaledRadialDeadZonedInput(float HorizonalInput, float VerticalInput);

	/**
	* @summary:	Sets the axis mapping information into oldBinding from newBinding.
	*
	* @param:	FInputAxisKeyMapping& oldBinding
	* @param:	FInputAxisKeyMapping& newBinding
	*
	* @return:	static void
	*/
	static void UpdateAxisMapping(FInputAxisKeyMapping& OldBinding, FInputAxisKeyMapping& NewBinding);

	/**
	* @summary:	Sets the action mapping information into oldBinding from newBinding.
	*
	* @param:	FInputActionKeyMapping& oldBinding
	* @param:	FInputActionKeyMapping& newBinding
	*
	* @return:	static void
	*/
	static void UpdateActionMapping(FInputActionKeyMapping& OldBinding, FInputActionKeyMapping& NewBinding);

	// Combat | Aiming ******************************************************

	void AimWeaponEnter();

	void AimWeaponExit();

	// Combat | Tabbing *****************************************************

	void ToggleWeapon();

	// Combat | Weapon ******************************************************

	void FirePrimaryWeaponEnter();

	void FirePrimaryWeaponExit();

	void FireSecondaryWeaponEnter();

	void FireSecondaryWeaponExit();

	void ReloadPrimaryWeapon();

	void ReloadSecondaryWeapon();

	// Combat | Grenade *****************************************************

	void ChargeGrenade();

	void ThrowGrenade();

	// Combat | Melee *******************************************************
	
	void Melee();

	// Movement | Base ******************************************************

	/**
	* @summary:	Moves the character's position that is connected to the controller on the vertical axis (forward/backward).
	*
	* @param:	float value
	*
	* @return:	void
	*/
	void MoveForward(float Value);

	/**
	* @summary:	Moves the character's position that is connected to the controller on the horizontal axis (left/right).
	*
	* @param:	float value
	*
	* @return:	void
	*/
	void MoveRight(float Value);

	/**
	* @summary:	Moves the character's aiming direction that is connected to the controller on the vertical axis (up/down).
	*
	* @param:	float value
	*
	* @return:	void
	*/
	void LookUp(float Value);

	/**
	* @summary:	Moves the character's aiming direction that is connected to the controller on the horizontal axis (left/right).
	*
	* @param:	float value
	*
	* @return:	void
	*/
	void LookRight(float Value);

	// Movement | Ability ***************************************************

	void Ability();

	// Movement | Clamber ***************************************************

	void Clamber();

	// Movement | Crouch ****************************************************

	void CrouchHoldEnter();

	void CrouchHoldExit();

	void CrouchToggle();

	// Movement | Hover *****************************************************

	void HoverEnter();

	void HoverExit();

	// Movement | Jump ******************************************************

	void Jump();

	// Movement | Slide *****************************************************

	void Slide();

	// Movement | Sprint ****************************************************

	void SprintEnter();

	void SprintExit();

public:

	// *** FUNCTIONS

	// Controller ***********************************************************

	UFUNCTION(BlueprintCallable, Category = "Controller")
		void GamepadRumble(float Intensity, float Duration, bool AffectsLeftLarge, bool AffectsLeftSmall, bool AffectsRightLarge, bool AffectsRightSmall);

	// Combat | Aiming ******************************************************

	UFUNCTION(BlueprintPure, Category = "Combat | Aiming")
		float GetDefaultFov() { return _fDefaultFov; }

	// Combat | Firing ******************************************************



	// Input | Rebinding ****************************************************

	/**
	* @summary:	Tries to find the matching axis binding & replaces the input key with the new binding.
	*
	* @param:	FInputAxisKeyMapping oldBinding
	* @param:	FInputAxisKeyMapping newBinding
	*
	* @return:	boolean
	*/
	UFUNCTION(BlueprintCallable, Category = "Input | Rebinding")
		bool RebindAxisKey(FInputAxisKeyMapping OldBinding, FInputAxisKeyMapping NewBinding);

	/**
	* @summary:	Tries to find the matching action binding & replaces the input key with the new binding.
	*
	* @param:	FInputActionKeyMapping oldBinding
	* @param:	FInputActionKeyMapping newBinding
	*
	* @return:	boolean
	*/
	UFUNCTION(BlueprintCallable, Category = "Input | Rebinding")
		bool RebindActionKey(FInputActionKeyMapping OldBinding, FInputActionKeyMapping NewBinding);

	UFUNCTION(exec)
		virtual void RemoveRoundFromChamber();

	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;

};
