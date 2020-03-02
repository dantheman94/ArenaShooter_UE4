// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Runtime/Core/Public/Math/Transform.h"
#include "ArenaCharacter.generated.h"

// *** ENUMS

UENUM(BlueprintType)
enum class E_Direction : uint8
{
	eGA_Fwd UMETA(DisplayName = "Forward"),
	eGA_FwdL UMETA(DisplayName = "Forward Left"),
	eGA_FwdR UMETA(DisplayName = "Forward Right"),
	eGA_Bwd UMETA(DisplayName = "Backward"),
	eGA_BwdL UMETA(DisplayName = "Backward Left"),
	eGA_BwdR UMETA(DisplayName = "Backward Right"),
	eGA_Left UMETA(DisplayName = "Left"),
	eGA_Right UMETA(DisplayName = "Right"),
	eGA_Idle UMETA(DisplayName = "Idle")
};

// *** STRUCTS

USTRUCT(BlueprintType)
struct FDashCameraShakeStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TSubclassOf<class UCameraShake> _CameraShake;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		E_Direction _eDirection;
	
	// Sets default values for this character's properties
	FDashCameraShakeStruct()
	{
		_CameraShake = NULL;
		_eDirection = E_Direction::eGA_Idle;
	}

	/*
	*
	*/
	bool GetCameraShakeByDirection(E_Direction Direction)
	{ return Direction == _eDirection; }

	/*
	*
	*/
	TSubclassOf<class UCameraShake> GetCameraShakeClass()
	{ return _CameraShake; }
};

// *** EVENT DISPATCHERS / DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDashAnimDelegate, E_Direction, Direction);

// *** CLASSES

UCLASS()
class ARENASHOOTER_API AArenaCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Startup ********************************************************************************************************************************

	/**
	* @summary:	Sets default values for this actor's properties.
	*
	* @return:	Constructor
	*/
	AArenaCharacter();

	///////////////////////////////////////////////

	~AArenaCharacter();

	///////////////////////////////////////////////

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

protected:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Startup ********************************************************************************************************************************

	/**
	* @summary:	Called when the game starts or when spawned.
	*
	* @return:	virtual void
	*/
	virtual void BeginPlay() override;

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// Movement | Dash ************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Dash")
		bool _bDashEnabled = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Dash")
		int _iDashStaminaChannel = 1;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Dash")
		float _fGroundDashStrength = 1350.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Dash")
		float _fGroundDashAngleStrength = 875.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Dash")
		float _fAirDashStrength = 1200.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Dash")
		float _fAirDashAngleStrength = 925.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Dash")
		TArray<FDashCameraShakeStruct> _DashCameraShakes;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Dash")
		float _fThrustGamepadRumbleIntensity = 3.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Dash")
		float _fThrustGamepadRumbleDuration = 0.15f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Dash")
		bool _fThrustGamepadRumbleAffectsLeftLarge = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Dash")
		bool _fThrustGamepadRumbleAffectsLeftSmall = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Dash")
		bool _fThrustGamepadRumbleAffectsRightLarge = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Dash")
		bool _fThrustGamepadRumbleAffectsRightSmall = true;

	/*
	*
	*/
	UPROPERTY(BlueprintAssignable, Category = "Movement | Dash")
		FDashAnimDelegate _fDashAnimationEvent;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement | Dash")
		FTransform _tFpArmsRelativeStarting;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement | Dash", Replicated)
		bool _bIsDashing = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement | Dash")
		bool _bCanDash = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement | Dash", Replicated)
		E_Direction _eDashDirection = E_Direction::eGA_Idle; // TO BE MADE DEPRECIATED

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Dash")
		float _fDashDuration = 2.0f;

	/*
	*	A timer handle used for referencing the dash.
	*/
	UPROPERTY()
		FTimerHandle _fDashHandle;

	// Movement | Double Jump ************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Double Jump")
		bool _bDoubleJumpEnabled = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Double Jump")
		int _iDoubleJumpStaminaChannel = 2;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Double Jump")
		float _fDoubleJumpForce = 820.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Double Jump")
		float _fDoubleJumpCameraShakeScale = 1.5f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement | Double Jump")
		bool _bCanDoubleJump = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement | Double Jump", Replicated)
		bool _bIsDoubleJumping = false;

	// Movement | Hover ***********************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Hover")
		bool _bHoverEnabled = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Hover")
		float _fHoverDuration = 3.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement | Hover", Replicated)
		bool _bIsHovering = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement | Hover")
		bool _bCanHover = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement | Hover")
		bool _bHoverCancelled = false;

	/*
	*	A timer handle used for referencing the hovering.
	*/
	UPROPERTY()
		FTimerHandle _fHoverHandle;

	// Movement | Slide ***********************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement | Slide", Replicated)
		bool _bIsSliding = false;

public:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Current Frame **************************************************************************************************************************

	/**
	* @summary:	Called every frame.
	*
	* @param:	float DeltaTime
	*
	* @return:	virtual void
	*/
	virtual void Tick(float DeltaTime) override;

	///////////////////////////////////////////////

	/*
	*
	*/
	virtual void OnGroundChecks() override;

	// Inventory | Weapon | Primary ***********************************************************************************************************

	/**
	* @summary:	Checks and initiates a reload of the character's primary weapon.
	*
	* @return:	virtual void
	*/
	virtual void InputReloadPrimaryWeapon() override;

	// Inventory | Weapon | Secondary *********************************************************************************************************

	/**
	* @summary:	Checks and initiates a reload of the character's secondary weapon.
	*
	* @return:	virtual void
	*/
	virtual void InputReloadSecondaryWeapon() override;

	///////////////////////////////////////////////

	/*

	*/
	virtual void InitFirePrimaryWeapon() override;

	// Movement | Base ************************************************************************************************************************

	/*
	*
	*/
	virtual void MoveForward(float Value) override;

	///////////////////////////////////////////////

	/*
	*
	*/
	virtual void MoveRight(float Value) override;

	///////////////////////////////////////////////

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0);

	///////////////////////////////////////////////

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_Reliable_SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0);

	// Movement | Dash ************************************************************************************************************************

	/*

	*/
	UFUNCTION(BlueprintCallable, Category = "Movement | Dash")
		void InputDash();

	///////////////////////////////////////////////

	/*

	*/
	UFUNCTION(Category = "Movement | Dash")
		void SetFPRelativeStartingTransform(FTransform Transform) { _tFpArmsRelativeStarting = Transform; }

	///////////////////////////////////////////////

	/*

	*/
	UFUNCTION(BlueprintCallable, Category = "Movement | Dash")
		E_Direction GetDirectionFromInput();

	///////////////////////////////////////////////

	/*

	*/
	UFUNCTION(Server, Reliable, WithValidation, Category = "Movement | Dash")
		void Server_Reliable_SetDashDirection(E_Direction Direction);

	///////////////////////////////////////////////

	/*

	*/
	UFUNCTION(Server, Reliable, WithValidation, Category = "Movement | Dash")
		void Server_Reliable_SetDashing(bool Dashing);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void StopDashing();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void DetermineFinalLaunchVelocity(FVector& LaunchVelocity, FVector InitialLaunchDirection);

	// Movement | Hover ***********************************************************************************************************************

	/*

	*/
	UFUNCTION(BlueprintCallable, Category = "Movement | Hover")
		void HoverEnter();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, Category = "Movement | Hover")
		void HoverExit();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetHovering(bool IsHovering);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_ChangeHoverState(bool IsHovering);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_Reliable_ChangeHoverState(bool IsHovering);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetGravityScale(float Scale);

	/*
	*
	*/
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_Reliable_SetGravityScale(float Scale);

	// Movement | Jump ************************************************************************************************************************

	/*

	*/
	virtual void InputJump() override;

	///////////////////////////////////////////////

	/*

	*/
	UFUNCTION()
		void InputDoubleJump();

	///////////////////////////////////////////////

	/**
	* @summary:	Sets the whether the character is double jumping or not.
	*
	* @networking:	Runs on server
	*
	* @param:	bool Jumping
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetDoubleJumping(bool DoubleJumping);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_LaunchCharacter(FVector LaunchVelocity, bool XYOverride, bool ZOverride);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_Reliable_LaunchCharacter(FVector LaunchVelocity, bool XYOverride, bool ZOverride);

};
