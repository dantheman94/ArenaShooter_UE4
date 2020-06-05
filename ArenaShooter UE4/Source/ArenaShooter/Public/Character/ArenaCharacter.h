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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWallRunAnimDelegate, E_WallRunDirection, WallRunSide);

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
	* @summary:	Constructor
	*/
	AArenaCharacter();

	/**
	* @summary:	Deconstructor
	*/
	~AArenaCharacter();

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
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Dash")
		FTransform _tFpArmsRelativeStarting;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Dash", Replicated)
		bool _bIsDashing = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Dash")
		bool _bCanDash = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Dash", Replicated)
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
		bool _bDoubleJumpRequiresStamina = false;

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
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Double Jump")
		bool _bCanDoubleJump = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Double Jump", Replicated)
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
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Hover", Replicated)
		bool _bIsHovering = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Hover")
		bool _bCanHover = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Hover")
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
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide")
		bool _bSlideEnabled = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement | Slide")
		TSubclassOf<class UCameraShake> _SlideStartCameraShake;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Slide")
		UCameraShake* _SlideCameraShakeInstance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Slide")
		bool _bIsTryingToSlide = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Slide", Replicated)
		bool _bIsSliding = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Slide")
		bool _bLerpSlideCamera = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Slide")
		bool _bSlideEnter = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Slide")
		FTransform _tSlideEnterOrigin;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide")
		float _fSlideForwardVelocityThreshold = 600.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide")
		float _fSlideUpVelocityThreshold = -500.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide")
		float _fSlideAirbourneGravityForce = 5.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide")
		FTransform _tSlideWeaponOrigin;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide")
		float _fSlideCameraLerpingDuration = 0.25f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Slide")
		float _fSlideCameraLerpTime = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide")
		float _fSlideForce = 300.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide")
		bool _fSlideLaunchXYOverride = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide")
		bool _fSlideLaunchZOverride = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide")
		float _fSlideBreakingFrictionFactor = 0.5f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide")
		float _fSlideBrakingDeceleration = 600.0f;

	// Movement | Slide Jump ************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide Jump")
		bool _bSlideJumpEnabled = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide Jump")
		bool _bSlideJumpRequiresStamina = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide Jump")
		int _iSlideJumpStaminaChannel = 2;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide Jump")
		float _fSlideJumpLaunchXForce = 100.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide Jump")
		float _fSlideJumpLaunchZForce = 700.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide Jump")
		bool _fSlideJumpLaunchXYOverride = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide Jump")
		bool _fSlideJumpLaunchZOverride = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Slide Jump")
		bool _bCanSlideJump = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Slide Jump", Replicated)
		bool _bIsSlideJumping = false;

	// Movement | Wall Running **********************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Wall Running")
		bool _bWallRunEnabled = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Wall Running")
		int _iWallRunStaminaChannel = 1.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Wall Running")
		float _fWallRunningOriginAdditive = 1.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Wall Running")
		float _fWallRunningRollMaximum = 20.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Wall Running")
		bool _bLerpWallRunCamera = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Wall Running", ReplicatedUsing = OnRep_IsWallRunning)
		bool _bIsWallRunning = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Wall Running")
		FVector _WallRunDirection = FVector::ZeroVector;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Wall Running", Replicated)
		E_WallRunDirection _eWallRunSide = E_WallRunDirection::eWRD_Left;

	// Movement | Wall Run Jump *********************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Wall Run Jump")
		bool _bIsTryingToWallJump = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Wall Run Jump")
		bool _bWallRunJumpXYOverride = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Wall Run Jump")
		float _fWallRunJumpLaunchXForce = 500.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Wall Run Jump")
		float _fWallRunJumpLaunchYForce = 300.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Wall Run Jump")
		float _fWallRunJumpLaunchZForce = 700.0f;

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
	*
	*/
	virtual void InputJump() override;

	/*
	*
	*/
	virtual void InputJumpExit();

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
	* @summary:	Launches the character with velocity in the specified direction.
	*
	* @networking:	Runs on server
	*
	* @param:	FVector LaunchVelocity
	* @param:	bool XYOverride
	* @param:	bool ZYOverride
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_LaunchCharacter(FVector LaunchVelocity, bool XYOverride, bool ZOverride);

	///////////////////////////////////////////////

	/*
	* @summary:	Launches the character with velocity in the specified direction.
	*
	* @networking:	Runs on all clients
	*
	* @param:	FVector LaunchVelocity
	* @param:	bool XYOverride
	* @param:	bool ZYOverride
	*
	* @return:	void
	*/
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_Reliable_LaunchCharacter(FVector LaunchVelocity, bool XYOverride, bool ZOverride);

	// Movement | Slide ***********************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		virtual void InputSlideEnter();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		virtual void InputSlideExit();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		virtual void InputSlideToggle(bool Sliding);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void Slide();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetIsSliding(bool Sliding);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_InitiateSlide();

	/*
	*
	*/
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_Reliable_InitiateSlide();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_StopSlide();

	/*
	*
	*/
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_Reliable_StopSlide();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Movement | Slide")
		bool IsSliding() { return _bIsSliding; }

	// Movement | Slide Jump ************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		void InputSlideJump();

	///////////////////////////////////////////////

	/**
	* @summary:	Sets the whether the character is slide jumping or not.
	*
	* @networking:	Runs on server
	*
	* @param:	bool Jumping
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetSlideJumping(bool SlideJumping);

	// Movement | Wall Running **********************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		virtual void OnRep_IsWallRunning();

	/*
	*
	*/
	UFUNCTION()
		void SetHorizontalVelocity(float HorizontalVelocityX, float HorizontalVelocityY);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetHorizontalVelocity(float HorizontalVelocityX, float HorizontalVelocityY);

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Movement | Wall Running")
		FVector2D GetHorizontalVelocity() const;

	/*
	*
	*/ 
	UFUNCTION()
		void ClampHorizontalVelocity();

	/*
	*
	*/
	UFUNCTION()
		FVector DetermineRunDirectionAndSide(FVector WallNormal, E_WallRunDirection& WallRunDirectionEnum);

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Movement | Wall Running")
		bool IsWallRunning() const { return _bIsWallRunning; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Movement | Wall Running")
		bool ValidWallRunInput() const;

	/*
	*
	*/
	UFUNCTION()
		bool CheckIfSurfaceCanBeWallRan(FVector SurfaceNormal) const;

	/*
	*
	*/
	UFUNCTION()
		void OnCapsuleComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/*
	*
	*/
	UFUNCTION()
		FVector GetWallRunLaunchVelocity();

	/*
	*
	*/
	UFUNCTION()
		void StartWallRun();

	/*
	*
	*/
	UFUNCTION()
		void EndWallRun();

	/*
	*
	*/
	UFUNCTION()
		void Tick_WallRunning();

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetIsWallRunning(bool IsWallRunning);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetWallRunDirection(E_WallRunDirection WallRunDirection);

};
