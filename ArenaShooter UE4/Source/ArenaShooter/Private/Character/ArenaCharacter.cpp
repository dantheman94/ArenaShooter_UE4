// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "FireMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetInputLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Stamina.h"
#include "UnrealNetwork.h"
#include "Weapon.h"

// Startup ********************************************************************************************************************************

/**
* @summary:	Sets default values for this actor's properties.
*
* @return:	Constructor
*/
AArenaCharacter::AArenaCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Actor replicates
	bReplicates = true;
	SetReplicateMovement(true);

}

///////////////////////////////////////////////

AArenaCharacter::~AArenaCharacter()
{

}

///////////////////////////////////////////////

void AArenaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaCharacter, _bIsDashing);
	DOREPLIFETIME(AArenaCharacter, _bIsDoubleJumping);
	DOREPLIFETIME(AArenaCharacter, _bIsHovering);
	DOREPLIFETIME(AArenaCharacter, _bIsSliding);
	DOREPLIFETIME(AArenaCharacter, _bIsWallRunning);
	DOREPLIFETIME(AArenaCharacter, _eDashDirection);
	DOREPLIFETIME(AArenaCharacter, _eWallRunSide);
}

///////////////////////////////////////////////

/**
* @summary:	Called when the game starts or when spawned.
*
* @return:	virtual void
*/
void AArenaCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get default variable values
	_fCameraRotationLagSpeed = _FirstPerson_SpringArm->CameraRotationLagSpeed;
	_fDefaultCapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	// Get default movement values
	_fDefaultAirControl = GetCharacterMovement()->AirControl;
	_fDefaultGravityScale = GetCharacterMovement()->GravityScale;
	_fDefaultGroundFriction = GetCharacterMovement()->GroundFriction;
	_fDefaultBrakingFrictionFactor = GetCharacterMovement()->BrakingFrictionFactor;
	_fDefaultBrakingDecelerationWalking = GetCharacterMovement()->BrakingDecelerationWalking;

	///GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Purple, TEXT("" + FString::SanitizeFloat(GetCharacterMovement()->GravityScale)));
	///GEngine->AddOnScreenDebugMessage(11, 5.0f, FColor::Orange, TEXT("" + FString::SanitizeFloat(_fDefaultGravityScale)));

	// Overwriting jump force in the character movement component
	if (_bOverwriteJumpMovementComponent)
	{
		// Get character movement component
		UCharacterMovementComponent* characterMovement = GetCharacterMovement();
		if (characterMovement == NULL) { return; }

		// Overwrite the jump force in the movement component
		characterMovement->JumpZVelocity = _fOverwriteJumpForce;
	}

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AArenaCharacter::OnCapsuleComponentHit);
}

// Current Frame **************************************************************************************************************************

/**
* @summary:	Called every frame.
*
* @param:	DeltaTime	- The delta of the frame.
*
* @return:	virtual void
*/
void AArenaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If we're currently in the air and not doing any checks for landing on the ground, then start doing those checks
	if (GetCharacterMovement()->IsFalling() && !_bIsPerformingGroundChecks) { _bIsPerformingGroundChecks; }
	if (_bIsPerformingGroundChecks) { OnGroundChecks(); }
	
	// Slide camera(origin) lerping
	if (_bLerpSlideCamera)
	{
		if (_PrimaryWeapon == NULL) { return; }

		// Add to lerp time
if (_fSlideCameraLerpTime < _fSlideCameraLerpingDuration)
{
	_fSlideCameraLerpTime += DeltaTime;

	// Determine origin transform
	bool ads = _PrimaryWeapon->GetCurrentFireMode()->IsAimDownSightEnabled();
	FTransform adsOrigin = _PrimaryWeapon->GetCurrentFireMode()->GetOriginADS();
	FTransform hipOrigin = _PrimaryWeapon->GetTransformOriginHands();
	FTransform originTransform = UKismetMathLibrary::SelectTransform(adsOrigin, hipOrigin, _bIsAiming && ads);

	// Get slide/unslide origins
	FTransform slideTransform = UKismetMathLibrary::SelectTransform(_tSlideWeaponOrigin, originTransform, _bIsSliding);
	FTransform unslideTransform = UKismetMathLibrary::SelectTransform(originTransform, _tSlideWeaponOrigin, _bIsSliding);

	// Current lerp transition origin between slide/unslide
	FTransform enter = UKismetMathLibrary::SelectTransform(slideTransform, unslideTransform, _bSlideEnter);
	FTransform exit = UKismetMathLibrary::SelectTransform(slideTransform, unslideTransform, !_bSlideEnter);
	FTransform lerpTransform = UKismetMathLibrary::TLerp(unslideTransform, slideTransform, _fSlideCameraLerpTime / _fSlideCameraLerpingDuration);

	// Set lerp transform to first person arms
	_FirstPerson_Arms->SetRelativeTransform(lerpTransform);
}

// Stop lerping
else
{ _bLerpSlideCamera = false; }
	}

	// Slide movement capsule lerping
	if (_bLerpCrouchCapsule)
	{
		// Add to lerp time
		if (_fSlideCameraLerpTime < _fCrouchLerpingDuration)
		{
			_fSlideCameraLerpTime += DeltaTime;

			// Get capsule height values
			float slideHeight = GetCharacterMovement()->CrouchedHalfHeight / 4;
			float lerpA = UKismetMathLibrary::SelectFloat(_fDefaultCapsuleHalfHeight, slideHeight, _bIsSliding);
			float lerpB = UKismetMathLibrary::SelectFloat(slideHeight, _fDefaultCapsuleHalfHeight, _bIsSliding);

			// Lerp between values over time
			float currentLerpHeight = UKismetMathLibrary::Lerp(lerpA, lerpB, _fSlideCameraLerpTime / _fSlideCameraLerpingDuration);
			UCapsuleComponent* movementCapsule = GetCapsuleComponent();
			if (movementCapsule == NULL) { return; }
			movementCapsule->SetCapsuleHalfHeight(currentLerpHeight, true);
		}
	}

	ClampHorizontalVelocity();
	if (_bIsWallRunning) { Tick_WallRunning(); }

	// Wall-run camera(origin) lerping
	if (_bLerpWallRunCamera)
	{
		// Tilt camera to wall run angle
		APlayerController* controller = Cast<APlayerController>(GetController());
		float roll = controller->GetControlRotation().Roll;
		switch (_eWallRunSide)
		{
		case E_WallRunDirection::eWRD_Left:
		{
			if (roll <= _fWallRunningRollMaximum + 1 || roll > 360 - _fWallRunningRollMaximum)
			{
				AddControllerRollInput(-_fWallRunningOriginAdditive);
			}
			break;
		}
		case E_WallRunDirection::eWRD_Right:
		{
			if (roll < _fWallRunningRollMaximum || roll >= 360 - _fWallRunningRollMaximum - 1)
			{
				AddControllerRollInput(_fWallRunningOriginAdditive);
			}
			break;
		}
		default: break;
		}

		///GEngine->AddOnScreenDebugMessage(5, 5.0f, FColor::Cyan, FString::SanitizeFloat(controller->GetControlRotation().Roll));
	}
	else
	{
		// Tilt camera back to zero
		APlayerController* controller = Cast<APlayerController>(GetController());
		float roll = controller->GetControlRotation().Roll;

		if (roll != 0)
		{
			if (roll >= 360 - _fWallRunningRollMaximum - 1) { AddControllerRollInput(_fWallRunningOriginAdditive); }
			if (roll <= _fWallRunningRollMaximum + 1) { AddControllerRollInput(-_fWallRunningOriginAdditive); }

			///GEngine->AddOnScreenDebugMessage(5, 5.0f, FColor::Cyan, FString::SanitizeFloat(controller->GetControlRotation().Roll));
		}
	}
}

///////////////////////////////////////////////

/**
* @summary:	Called every frame.
*
* @param:	DeltaTime	- The delta of the frame.
*
* @return:	virtual void
*/
void AArenaCharacter::OnGroundChecks()
{
	// Character has landed on the ground
	if (GetCharacterMovement()->IsMovingOnGround()) 
	{
		// Reset jumping
		if (_bIsJumping)
		{
			if (GetLocalRole() == ROLE_Authority) { _bIsJumping = false; } else { Server_Reliable_SetJumping(false); }
		}
		if (_bIsDoubleJumping)
		{
			if (GetLocalRole() == ROLE_Authority) { _bIsDoubleJumping = false; } else { Server_Reliable_SetDoubleJumping(false); }
		}
		if (_bDoubleJumpEnabled) { _bCanDoubleJump = true; }
		if (_bIsSlideJumping)
		{
			if (GetLocalRole() == ROLE_Authority) { _bIsSlideJumping = false; } else { Server_Reliable_SetDoubleJumping(false); }
		}

		// Set gravity scale to default
		if (GetLocalRole() == ROLE_Authority)
		{ Multicast_Reliable_SetGravityScale(_fDefaultGravityScale); } 
		else
		{ Server_Reliable_SetGravityScale(_fDefaultGravityScale); }
		///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, TEXT("" + FString::SanitizeFloat(_fDefaultGravityScale)));

		// Reset falling/hover
		_bCanHover = true;
		_bHoverCancelled = false;

		// Launch the character if we were trying to slide and waiting to hit the ground first
		if (_bIsTryingToSlide) { Slide(); }
		
		// Dont play the landing camera shake if we're trying to slide
		else
		{
			// Landing camera shake
			float shakeScale = UKismetMathLibrary::NormalizeToRange(_fFallingVelocity *= -1, 0.0f, 2000.0f);
			shakeScale *= _fFallingLandShakeMultiplier;
			OwningClient_PlayCameraShake(_CameraShakeJumpLand, shakeScale);
		}
		_bIsPerformingGroundChecks = false;
	}

	// Character is falling
	else { _fFallingVelocity = GetVelocity().Z; }
}

// Inventory | Weapon | Primary ***********************************************************************************************************

/**
* @summary:	Checks and initiates a reload of the character's primary weapon.
*
* @return:	virtual void
*/
void AArenaCharacter::InputReloadPrimaryWeapon()
{
	if (_bIsDashing) { return; }

	ABaseCharacter::InputReloadPrimaryWeapon();
}

// Inventory | Weapon | Secondary *********************************************************************************************************

/**
* @summary:	Checks and initiates a reload of the character's secondary weapon.
*
* @return:	virtual void
*/
void AArenaCharacter::InputReloadSecondaryWeapon()
{
	if (_bIsDashing) { return; }

	ABaseCharacter::InputReloadSecondaryWeapon();
}

///////////////////////////////////////////////

void AArenaCharacter::InitFirePrimaryWeapon()
{
	// Cant fire if performing certain movements
	if (_bIsDashing) { return; }

	ABaseCharacter::InitFirePrimaryWeapon();
}

// Movement | Base ************************************************************************************************************************

/**
* @summary:	Moves the character on the vertical axis (forward/backward)
*
* @return:	void
*/
void AArenaCharacter::MoveForward(float Value)
{
	if (!_bIsSliding) { ABaseCharacter::MoveForward(Value); }
}

///////////////////////////////////////////////

/**
* @summary:	Moves the character on the horizontal axis (left/right)
*
* @return:	void
*/
void AArenaCharacter::MoveRight(float Value)
{
	if (!_bIsSliding) { ABaseCharacter::MoveRight(Value); }
}

///////////////////////////////////////////////

bool AArenaCharacter::Server_Reliable_SetMovementMode_Validate(EMovementMode NewMovementMode, uint8 NewCustomMode = 0)
{ return true; }

void AArenaCharacter::Server_Reliable_SetMovementMode_Implementation(EMovementMode NewMovementMode, uint8 NewCustomMode = 0)
{
	Multicast_Reliable_SetMovementMode(NewMovementMode, NewCustomMode);
}

///////////////////////////////////////////////

bool AArenaCharacter::Multicast_Reliable_SetMovementMode_Validate(EMovementMode NewMovementMode, uint8 NewCustomMode = 0)
{ return true; }

void AArenaCharacter::Multicast_Reliable_SetMovementMode_Implementation(EMovementMode NewMovementMode, uint8 NewCustomMode = 0)
{
	UCharacterMovementComponent* movementComp = GetCharacterMovement();
	if (movementComp == NULL) { return; }

	movementComp->SetMovementMode(NewMovementMode, NewCustomMode);
}

// Movement | Dash ************************************************************************************************************************

/*
*
*/
void AArenaCharacter::InputDash()
{
	if (_bDashEnabled && _bCanDash && _uStaminaComponents.Num() > 0)
	{
		// Get relevant stamina via matching channel
		UStamina* stamina = NULL;
		for (int i = 0; i < _uStaminaComponents.Num(); i++)
		{
			if (_uStaminaComponents[i]->GetStaminaChannel() == _iDashStaminaChannel)
			{
				stamina = _uStaminaComponents[i];
				break;
			}
		}

		// Valid stamina channel found
		if (stamina != NULL)
		{
			if (stamina->IsFullyRecharged())
			{
				// Drain stamina
				stamina->SetStamina(0.0f);
				stamina->DelayedRecharge();

				// Cancel/disable combat modes
				_bPrimaryReloadCancelled = true;
				_bSecondaryReloadCancelled = true;
				_bCanFirePrimary = false;
				_bCanFireSecondary = false;

				// Stop any current movement modes
				if (_bIsAiming) { AimWeaponExit(); }
				if (_bIsCrouching) { CrouchToggle(_bIsCrouching); }
				if (_bIsHovering) { HoverExit(); }

				// Gamepad rumble
				OwningClient_GamepadRumble(_fThrustGamepadRumbleIntensity, _fThrustGamepadRumbleDuration,
					_fThrustGamepadRumbleAffectsLeftLarge, _fThrustGamepadRumbleAffectsLeftSmall,
					_fThrustGamepadRumbleAffectsRightLarge, _fThrustGamepadRumbleAffectsRightSmall);

				// Get input WASD dash direction
				Server_Reliable_SetDashDirection(GetDirectionFromInput());
				///FString _Message = TEXT("_eDashDirection is: ") + EnumToString(TEXT("E_Direction"), static_cast<uint8>(_eDashDirection));
				///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, _Message);

				// Create dash launch vector based off direction
				FVector* launchVector = new FVector(FVector::ZeroVector);
				UCharacterMovementComponent* moveComponent = GetCharacterMovement();
				moveComponent->GroundFriction = 0.0f;

				// Air dash velocity
				if (moveComponent->IsFalling())
				{
					switch (GetDirectionFromInput())
					{

					case E_Direction::eGA_Idle:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(_fAirDashStrength, 0.0f, 0.0f));
						break;
					}
					case E_Direction::eGA_Fwd:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(_fAirDashStrength, 0.0f, 0.0f));
						break;
					}
					case E_Direction::eGA_FwdL:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(_fAirDashAngleStrength, 0.0f - _fAirDashAngleStrength, 0.0f));
						break;
					}
					case E_Direction::eGA_FwdR:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(_fAirDashAngleStrength, _fAirDashAngleStrength, 0.0f));
						break;
					}
					case E_Direction::eGA_Bwd:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(0.0f - _fAirDashStrength, 0.0f, 0.0f));
						break;
					}
					case E_Direction::eGA_BwdL:
					{
						 DetermineFinalLaunchVelocity(*launchVector, FVector(0.0f - _fAirDashAngleStrength, 0.0f - _fAirDashAngleStrength, 0.0f));
						break;
					}
					case E_Direction::eGA_BwdR:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(0.0f - _fAirDashAngleStrength, _fAirDashAngleStrength, 0.0f));
						break;
					}
					case E_Direction::eGA_Left:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(0.0f, 0.0f - _fAirDashStrength, 0.0f));
						break;
					}
					case E_Direction::eGA_Right:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(0.0f, _fAirDashStrength, 0.0f));
						break;
					}
					default: break;
					}
				}

				// Ground dash velocity
				else
				{
					switch (GetDirectionFromInput())
					{

					case E_Direction::eGA_Idle:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(_fGroundDashStrength, 0.0f, 0.0f));
						break;
					}
					case E_Direction::eGA_Fwd:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(_fGroundDashStrength, 0.0f, 0.0f));
						break;
					}
					case E_Direction::eGA_FwdL:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(_fGroundDashAngleStrength, 0.0f - _fGroundDashAngleStrength, 0.0f));
						break;
					}
					case E_Direction::eGA_FwdR:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(_fGroundDashAngleStrength, _fGroundDashAngleStrength, 0.0f));
						break;
					}
					case E_Direction::eGA_Bwd:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(0.0f - _fGroundDashStrength, 0.0f, 0.0f));
						break;
					}
					case E_Direction::eGA_BwdL:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(0.0f - _fGroundDashAngleStrength, 0.0f - _fGroundDashAngleStrength, 0.0f));
						break;
					}
					case E_Direction::eGA_BwdR:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(0.0f - _fGroundDashAngleStrength, _fGroundDashAngleStrength, 0.0f));
						break;
					}
					case E_Direction::eGA_Left:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(0.0f, 0.0f - _fGroundDashStrength, 0.0f));
						break;
					}
					case E_Direction::eGA_Right:
					{
						DetermineFinalLaunchVelocity(*launchVector, FVector(0.0f, _fGroundDashStrength, 0.0f));
						break;
					}
					default: break;
					}
				}

				// Dash
				if (HasAuthority())
				{ Multicast_Reliable_LaunchCharacter(*launchVector, true, true); }
				else
				{ Server_Reliable_LaunchCharacter(*launchVector, true, true); }
				
				// Start Dash timer
				Server_Reliable_SetDashing(true);
				FTimerDelegate dashDelegate;
				dashDelegate.BindUFunction(this, FName("StopDashing"));
				GetWorld()->GetTimerManager().SetTimer(_fDashHandle, dashDelegate, 1.0f, false, _fDashDuration);

				// Camera shake
				if (_DashCameraShakes.Num() > 0)
				{
					// Find matching camera shake by dash direction
					for (int i = 0; i < _DashCameraShakes.Num(); i++)
					{
						if (_DashCameraShakes[i].GetCameraShakeByDirection(GetDirectionFromInput()))
						{
							// Matching direction found -> play camera shake
							OwningClient_PlayCameraShake(_DashCameraShakes[i].GetCameraShakeClass(), 1.0f);
						}
					}
				}

				// Play FP animation for the dash
				_fDashAnimationEvent.Broadcast(GetDirectionFromInput());

				// Used a pointer for the launch vector, resources need to be cleared up 
				delete launchVector;

				// Stop shooting (if we were firing)
				if (_PrimaryWeapon != NULL)
				{
					if (_PrimaryWeapon->GetCurrentFireMode()->IsFiring())
					{
						InputPrimaryFireRelease();
					}
				}
			}
		}

		// stamina == NULL
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("ERROR: Cannot < Dash > due to _uStaminaComponents not finding a valid matching stamina channel."));
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
E_Direction AArenaCharacter::GetDirectionFromInput()
{
	E_Direction dir = E_Direction::eGA_Idle;

	// Get player controller ref
	auto ctrl = Cast<APlayerController>(this->GetController());
	
	// Forward
	if (ctrl->IsInputKeyDown(EKeys::W) || ctrl->IsInputKeyDown(EKeys::Gamepad_LeftStick_Up))
	{	
		// Left
		if (ctrl->IsInputKeyDown(EKeys::A) || ctrl->IsInputKeyDown(EKeys::Gamepad_LeftStick_Left))
		{ dir = E_Direction::eGA_FwdL; }

		// Right
		else if (ctrl->IsInputKeyDown(EKeys::D) || ctrl->IsInputKeyDown(EKeys::Gamepad_LeftStick_Right))
		{ dir = E_Direction::eGA_FwdR; }

		// Center
		else
		{ dir = E_Direction::eGA_Fwd; }
	} 

	// Not forward
	else 
	{
		// Backward
		if (ctrl->IsInputKeyDown(EKeys::S) || ctrl->IsInputKeyDown(EKeys::Gamepad_LeftStick_Down))
		{
			// Left
			if (ctrl->IsInputKeyDown(EKeys::A) || ctrl->IsInputKeyDown(EKeys::Gamepad_LeftStick_Left))
			{ dir = E_Direction::eGA_BwdL; }

			// Right
			else if (ctrl->IsInputKeyDown(EKeys::D) || ctrl->IsInputKeyDown(EKeys::Gamepad_LeftStick_Right))
			{ dir = E_Direction::eGA_BwdR; }

			// Center
			else
			{ dir = E_Direction::eGA_Bwd; }
			
		}

		// Not backward
		else
		{
			// Left
			if (ctrl->IsInputKeyDown(EKeys::A) || ctrl->IsInputKeyDown(EKeys::Gamepad_LeftStick_Left))
			{ dir = E_Direction::eGA_Left; }

			// Right
			if (ctrl->IsInputKeyDown(EKeys::D) || ctrl->IsInputKeyDown(EKeys::Gamepad_LeftStick_Right))
			{ dir = E_Direction::eGA_Right; }
		}
	}

	return dir;
}

///////////////////////////////////////////////

bool AArenaCharacter::Server_Reliable_SetDashDirection_Validate(E_Direction Direction)
{ return true; }

void AArenaCharacter::Server_Reliable_SetDashDirection_Implementation(E_Direction Direction)
{ _eDashDirection = Direction; }

///////////////////////////////////////////////

bool AArenaCharacter::Server_Reliable_SetDashing_Validate(bool Dashing)
{ return HasAuthority(); }

void AArenaCharacter::Server_Reliable_SetDashing_Implementation(bool Dashing)
{ _bIsDashing = Dashing; }

///////////////////////////////////////////////

/*
*
*/
void AArenaCharacter::StopDashing()
{
	// Reset friction
	GetCharacterMovement()->GroundFriction = _fJogGroundFriction;

	Server_Reliable_SetDashing(false);

	// Set weapon firing capabilities
	_bCanFirePrimary = true;
	_bCanFireSecondary = _bIsDuelWielding; // Can only fire secondary weapon if we're currently duel-wielding
}

///////////////////////////////////////////////

/*
*
*/
void AArenaCharacter::DetermineFinalLaunchVelocity(FVector& LaunchVelocity, FVector InitialLaunchDirection)
{
	float yaw = GetCharacterMovement()->GetPawnOwner()->GetControlRotation().Yaw;
	FQuat quat = FQuat::MakeFromEuler(FVector(0.0f, 0.0f, yaw));

	LaunchVelocity = UKismetMathLibrary::Quat_RotateVector(quat, InitialLaunchDirection);

	if (GetVelocity().Z >= 0.0f)
	{ LaunchVelocity = FVector(LaunchVelocity.X, LaunchVelocity.Y, LaunchVelocity.Z + 150.0f); }
	else
	{ LaunchVelocity = FVector(LaunchVelocity.X, LaunchVelocity.Y, LaunchVelocity.Z - 100.0f); }
}

// Movement | Hover ***********************************************************************************************************************

/*
*
*/
void AArenaCharacter::HoverEnter()
{
	// Possible to hover?
	if (_bHoverEnabled && _bCanHover && !_bIsDuelWielding)
	{
		UCharacterMovementComponent* moveComp = GetCharacterMovement();
		if (moveComp == NULL) { return; }

		// Can only hover if we're currently in the air
		if (moveComp->IsFalling())
		{
			// Stop jumping if we are
			if (_bIsJumping)
			{
				if (GetLocalRole() == ROLE_Authority)
				{ _bIsJumping = false; }
				else 
				{ Server_Reliable_SetJumping(false); }
			}

			// Stop double jumping if we are
			if (_bIsDoubleJumping)
			{
				if (GetLocalRole() == ROLE_Authority)
				{ _bIsDoubleJumping = false; }
				else
				{ Server_Reliable_SetDoubleJumping(false); }
			}		

			// Start hovering
			if (GetLocalRole() == ROLE_Authority)
			{ 
				_bIsHovering = true;
				Multicast_Reliable_ChangeHoverState(true); 
			}
			else
			{ 
				Server_Reliable_SetHovering(true);
				Server_Reliable_ChangeHoverState(true); 
			}

			// Cannot perform air control maneuverabilities
			moveComp->AirControl = 0.0f;

			_bCanHover = false;
			_bHoverCancelled = false;

			FTimerDelegate hoverDelegate;
			hoverDelegate.BindUFunction(this, FName("HoverExit"));
			GetWorld()->GetTimerManager().SetTimer(_fHoverHandle, hoverDelegate, 1.0f, false, _fHoverDuration);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AArenaCharacter::HoverExit()
{
	if (_bIsHovering)
	{
		_bHoverCancelled = true;

		// Landing on ground checks [ updates on Tick() ]
		_bIsPerformingGroundChecks = true;

		// Reset gravity back to default
		if (GetLocalRole() == ROLE_Authority)
		{ Multicast_Reliable_SetGravityScale(_fDefaultGravityScale); }
		else
		{ Server_Reliable_SetGravityScale(_fDefaultGravityScale); }

		// Stop hovering
		if (GetLocalRole() == ROLE_Authority)
		{ 
			_bIsHovering = false;
			Multicast_Reliable_ChangeHoverState(false); 
		}
		else
		{ 
			Server_Reliable_SetHovering(false);
			Server_Reliable_ChangeHoverState(false); 
		}

		// Reset air control maneuverabilities
		GetCharacterMovement()->AirControl = _fDefaultAirControl;
	}
}

///////////////////////////////////////////////

bool AArenaCharacter::Server_Reliable_SetHovering_Validate(bool IsHovering)
{ return true; }

void AArenaCharacter::Server_Reliable_SetHovering_Implementation(bool IsHovering)
{
	_bIsHovering = IsHovering;
}

///////////////////////////////////////////////

bool AArenaCharacter::Server_Reliable_ChangeHoverState_Validate(bool IsHovering)
{ return true; }

void AArenaCharacter::Server_Reliable_ChangeHoverState_Implementation(bool IsHovering)
{
	Multicast_Reliable_ChangeHoverState(IsHovering);
}

///////////////////////////////////////////////

bool AArenaCharacter::Multicast_Reliable_ChangeHoverState_Validate(bool IsHovering)
{ return true; }

void AArenaCharacter::Multicast_Reliable_ChangeHoverState_Implementation(bool IsHovering)
{
	// Hovering (Flying :O)
	if (IsHovering)
	{ Server_Reliable_SetMovementMode(MOVE_Flying); }

	// Not hovering
	else
	{
		UCharacterMovementComponent* moveComp = GetCharacterMovement();
		if (moveComp == NULL) { return; }

		// Falling or jumping
		if (moveComp->IsFalling())
		{ Server_Reliable_SetMovementMode(MOVE_Falling); 
		
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("BEPIS"));
		}

		// Not falling
		else
		{ Server_Reliable_SetMovementMode(MOVE_Walking); }
	}
}

///////////////////////////////////////////////

bool AArenaCharacter::Server_Reliable_SetGravityScale_Validate(float Scale)
{ return true; }

void AArenaCharacter::Server_Reliable_SetGravityScale_Implementation(float Scale)
{
	Multicast_Reliable_SetGravityScale(Scale);
}

///////////////////////////////////////////////

bool AArenaCharacter::Multicast_Reliable_SetGravityScale_Validate(float Scale)
{ return true; }

void AArenaCharacter::Multicast_Reliable_SetGravityScale_Implementation(float Scale)
{
	GetCharacterMovement()->GravityScale = Scale;
}

// Movement | Jump ************************************************************************************************************************

/*
*
*/
void AArenaCharacter::InputJump()
{
	InputVault();

	if (_bCanJump && !_bIsTryingToVault)
	{
		// Sanity check
		if (GetCharacterMovement() == NULL) { return; } 
		else
		{
			// Not falling
			if (!GetCharacterMovement()->IsFalling())
			{
				if (_bIsSliding)
				{
					InputSlideJump();
				}
				else
				{
					// Uncrouch then jump
					if (_bIsCrouching) { ExitCrouch(); } else
					{
						// Set _bIsJumping = TRUE
						if (GetLocalRole() == ROLE_Authority)
						{ _bIsJumping = true; } else
						{ Server_Reliable_SetJumping(true); }

						// Action jump
						Jump();

						// Play feedback(s) [Camera shakes / Gamepad Rumbles]
						OwningClient_PlayCameraShake(_CameraShakeJumpStart, 1.0f);
						OwningClient_GamepadRumble(_fJumpGamepadRumbleIntensity, _fJumpGamepadRumbleDuration,
							_fJumpGamepadRumbleAffectsLeftLarge, _fJumpGamepadRumbleAffectsLeftSmall,
							_fJumpGamepadRumbleAffectsRightLarge, _fJumpGamepadRumbleAffectsRightSmall);
					}

					// Unhover
					if (_bIsHovering)
					{
						HoverExit();
						InputDoubleJump();
					}
				}

			}
					   
			// Falling
			else { 

				_bIsTryingToWallJump = true;

				// Wall running is technically falling in the movement component
				if (IsWallRunning()) { EndWallRun();}
				else
				{ InputDoubleJump(); }
			}

			// Start landing checks
			_bIsPerformingGroundChecks = true;
		}
	}
}

/*
*
*
*/
void AArenaCharacter::InputJumpExit()
{
	_bIsTryingToWallJump = false;
}

///////////////////////////////////////////////

/*
*
*/
void AArenaCharacter::InputDoubleJump()
{
	InputVault();

	if (_bDoubleJumpEnabled && _bCanDoubleJump &&  !IsTryingToVault() && _uStaminaComponents.Num() > 0)
	{
		// Get relevant stamina via matching channel
		UStamina* stamina = NULL;
		bool canDoubleJump = true;
		if (_bDoubleJumpRequiresStamina)
		{
			for (int i = 0; i < _uStaminaComponents.Num(); i++)
			{
				if (_uStaminaComponents[i]->GetStaminaChannel() == _iDoubleJumpStaminaChannel)
				{
					stamina = _uStaminaComponents[i];
					break;
				}
			}
		}

		// Valid stamina channel found
		if (stamina != NULL) { canDoubleJump = stamina->IsFullyRecharged(); }
		else
		{
			// Didn't find a valid stamina channel & we require one
			if (_bDoubleJumpRequiresStamina)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
					TEXT("ERROR: Cannot < Double Jump > due to _uStaminaComponents not finding a valid matching stamina channel."));
				return;
			}
		}

		if (canDoubleJump)
		{
			// Drain stamina
			if (stamina != NULL)
			{
				if (stamina->IsFullyRecharged())
				{
					stamina->SetStamina(0.0f);
					stamina->DelayedRecharge();
				}
			}

			// Double jump
			_bCanDoubleJump = false;
			FVector force = FVector(0.0f, 0.0f, _fDoubleJumpForce);
			if (GetLocalRole() == ROLE_Authority)
			{
				LaunchCharacter(force, false, true);
				_bIsDoubleJumping = true;
			} else
			{
				Server_Reliable_LaunchCharacter(force, false, true);
				Server_Reliable_SetDoubleJumping(true);
			}

			// Start landing checks
			_bIsPerformingGroundChecks = true;

			// Play feedback(s) [Camera shakes / Gamepad Rumbles]
			OwningClient_PlayCameraShake(_CameraShakeJumpStart, _fDoubleJumpCameraShakeScale);
			OwningClient_GamepadRumble(_fThrustGamepadRumbleIntensity, _fThrustGamepadRumbleDuration,
				_fThrustGamepadRumbleAffectsLeftLarge, _fThrustGamepadRumbleAffectsLeftSmall,
				_fThrustGamepadRumbleAffectsRightLarge, _fThrustGamepadRumbleAffectsRightSmall);
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
bool AArenaCharacter::Server_Reliable_LaunchCharacter_Validate(FVector LaunchVelocity, bool XYOverride, bool ZOverride)
{ return true; }

void AArenaCharacter::Server_Reliable_LaunchCharacter_Implementation(FVector LaunchVelocity, bool XYOverride, bool ZOverride)
{
	Multicast_Reliable_LaunchCharacter(LaunchVelocity, XYOverride, ZOverride);
}

/*
*
*/
bool AArenaCharacter::Multicast_Reliable_LaunchCharacter_Validate(FVector LaunchVelocity, bool XYOverride, bool ZOverride)
{ return true; }

void AArenaCharacter::Multicast_Reliable_LaunchCharacter_Implementation(FVector LaunchVelocity, bool XYOverride, bool ZOverride)
{
	LaunchCharacter(LaunchVelocity, XYOverride, ZOverride);
}

// Movement | Jump ************************************************************************************************************************

#pragma region Server_Reliable_SetDoubleJumping

bool AArenaCharacter::Server_Reliable_SetDoubleJumping_Validate(bool DoubleJumping)
{ return true; }

void AArenaCharacter::Server_Reliable_SetDoubleJumping_Implementation(bool DoubleJumping)
{
	_bIsDoubleJumping = DoubleJumping;
}

#pragma endregion

// Movement | Slide ***********************************************************************************************************************

/*
*
*/
void AArenaCharacter::InputSlideEnter()
{
	if (_bSlideEnabled/* && !_bIsDashing*/)
	{
		// Can only slide when we are projecting forwards(relative to actors rotation) and downwards(world absolute) in velocity to a certain amount
		FVector velocity = GetCharacterMovement()->Velocity;
		float forwardSpeed = FVector::DotProduct(velocity, UKismetMathLibrary::GetForwardVector(GetActorRotation()));
		///bool checkUpVelocity = GetCharacterMovement()->IsMovingOnGround() ? true : velocity.Z < _fSlideUpVelocityThreshold;
		///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("" + FString::SanitizeFloat(forwardSpeed)));
		if (forwardSpeed > _fSlideForwardVelocityThreshold)
		{
			// If we're currently airbourne
			if (!GetCharacterMovement()->IsMovingOnGround())
			{
				// Fire trace to fall and check the distance of that trace
				GetCharacterMovement()->GravityScale = _fSlideAirbourneGravityForce;
				_bIsPerformingGroundChecks = true;
				_bIsTryingToSlide = true;
			} 
			
			// Already touching the ground so just slide straight away
			else { Slide(); }

			// Disable vaulting during a slide attempt
			_bCanVault = false;
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
void AArenaCharacter::InputSlideExit()
{
	// Can only stop sliding if we're currently sliding
	if (_bIsSliding)
	{
		// Set _bSliding to FALSE
		bool slide = false;
		if (GetLocalRole() == ROLE_Authority) { _bIsSliding = slide; } else { Server_Reliable_SetIsSliding(slide); }

		// Stop slide
		if (GetLocalRole() != ROLE_Authority) { Multicast_Reliable_StopSlide(); } else { Server_Reliable_StopSlide(); }

		// Lerp camera origin transform?
		_bSlideEnter = false;
		if (_fSlideCameraLerpTime != 0.0f)
		{
			if (_fSlideCameraLerpTime >= _fSlideCameraLerpingDuration) { _fSlideCameraLerpTime = 0.0f; } else
			{
				// Get current percent of lerp time so that it doesn't jagger at the start of the next lerp sequence
				float t = _fSlideCameraLerpTime;
				_fSlideCameraLerpTime = _fSlideCameraLerpingDuration - t;
			}
		}
		if (_PrimaryWeapon != NULL)
		{
			if (_PrimaryWeapon->GetCurrentFireMode()->IsAimDownSightEnabled())
			{ _bLerpSlideCamera = !IsAiming(); } else
			{ _bLerpSlideCamera = true; }
		} else { _bLerpSlideCamera = true; }
		_bLerpCrouchCapsule = _bLerpSlideCamera;

		// Stop camera shake
		if (_SlideCameraShakeInstance != NULL) { _SlideCameraShakeInstance->StopShake(false); }

		// Can no longer slide jump
		_bCanSlideJump = false;
		_bCanJump = true;

		// Enable vaulting
		_bCanVault = true;
	}
	_bIsTryingToSlide = false;
}

///////////////////////////////////////////////

/*
*
*/
void AArenaCharacter::InputSlideToggle(bool Sliding)
{
	// Slide enter
	if (Sliding)
	{
		if (!_bIsSliding) { InputSlideEnter(); 
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Slide Enter!"));
		}
	}

	// Slide exit
	else
	{
		InputSlideExit(); GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Slide Exit!"));
	}
}

///////////////////////////////////////////////

/*
*
*/
void AArenaCharacter::Slide()
{
	// Can only slide when we're using forward input
	APlayerController* playerController = Cast<APlayerController>(this->GetController());
	if (playerController == NULL) { return; }
	if (playerController->IsInputKeyDown(EKeys::W))
	{
		// Stop sprinting (if we were)
		if (_bIsSprinting) { StopSprinting(); }

		// Can shoot weapons again
		_bCanFirePrimary = true;
		if (_SecondaryWeapon != NULL) { _bCanFireSecondary = true; }

		// If we're not currently sliding
		if (!_bIsSliding)
		{
			// Set _bSliding to TRUE
			bool slide = true;
			if (GetLocalRole() == ROLE_Authority) { _bIsSliding = slide; } else { Server_Reliable_SetIsSliding(slide); }

			// Initiate sliding
			if (GetLocalRole() != ROLE_Authority) { Multicast_Reliable_InitiateSlide(); } else { Server_Reliable_InitiateSlide(); }

			// Local camera shake
			if (playerController != NULL && _SlideStartCameraShake != NULL)
			{
				_SlideCameraShakeInstance = playerController->PlayerCameraManager->PlayCameraShake(_SlideStartCameraShake, 1.0f, ECameraAnimPlaySpace::CameraLocal, FRotator::ZeroRotator);
			}
		}

		// Lerp camera origin transform?
		_tSlideEnterOrigin = _FirstPerson_Arms->GetRelativeTransform();
		_bSlideEnter = true;
		if (_fSlideCameraLerpTime != 0.0f)
		{
			if (_fSlideCameraLerpTime >= _fSlideCameraLerpingDuration) { _fSlideCameraLerpTime = 0.0f; } else
			{
				// Get current percent of lerp time so that it doesn't jagger at the start of the next lerp sequence
				float t = _fCrouchCameraLerpTime;
				_fSlideCameraLerpTime = _fSlideCameraLerpingDuration - t;
			}
		}
		if (_PrimaryWeapon != NULL)
		{
			if (_PrimaryWeapon->GetCurrentFireMode()->IsAimDownSightEnabled())
			{ _bLerpSlideCamera = !IsAiming(); } else
			{ _bLerpSlideCamera = true; }
		} else { _bLerpSlideCamera = true; }
		_bLerpCrouchCapsule = _bLerpSlideCamera;
	}

	// Allow slide jumping whilst we are sliding
	if (_bSlideJumpEnabled) { _bCanSlideJump = true; }
	_bCanJump = true;

	// Disable vaulting
	_bCanVault = false;
}

///////////////////////////////////////////////

/*
*
*/
bool AArenaCharacter::Server_Reliable_SetIsSliding_Validate(bool Sliding)
{ return true; }

void AArenaCharacter::Server_Reliable_SetIsSliding_Implementation(bool Sliding)
{
	_bIsSliding = Sliding;
}

///////////////////////////////////////////////

/*
*
*/
bool AArenaCharacter::Server_Reliable_InitiateSlide_Validate()
{ return true; }

void AArenaCharacter::Server_Reliable_InitiateSlide_Implementation()
{
	Multicast_Reliable_InitiateSlide();
}

/*
*
*/
bool AArenaCharacter::Multicast_Reliable_InitiateSlide_Validate()
{ return true; }

void AArenaCharacter::Multicast_Reliable_InitiateSlide_Implementation()
{
	// Get movement component
	UCharacterMovementComponent* movement = GetCharacterMovement();
	if (movement == NULL) { return; }

	// Only slide launch if we're touching the ground (so airbourne slides will wait till you hit the ground before launching you)
	if (movement->IsMovingOnGround())
	{
		// Set slide values
		movement->GroundFriction = 0.0f;
		movement->BrakingFrictionFactor = _fSlideBreakingFrictionFactor;
		movement->BrakingDecelerationWalking = _fSlideBrakingDeceleration;

		// Launch the character
		FVector forwardForce = GetActorForwardVector() * _fSlideForce;
		FVector downForce = GetActorUpVector() * (_fSlideForce * -1.0f);
		FVector launchForce = forwardForce + downForce;
		this->LaunchCharacter(launchForce, _fSlideLaunchXYOverride, _fSlideLaunchZOverride);
	}
}

///////////////////////////////////////////////

/*
*
*/
bool AArenaCharacter::Server_Reliable_StopSlide_Validate()
{ return true; }

void AArenaCharacter::Server_Reliable_StopSlide_Implementation()
{
	Multicast_Reliable_StopSlide();
}

/*
*
*/
bool AArenaCharacter::Multicast_Reliable_StopSlide_Validate()
{ return true; }

void AArenaCharacter::Multicast_Reliable_StopSlide_Implementation()
{
	// Get movement component
	UCharacterMovementComponent* movement = GetCharacterMovement();
	if (movement == NULL) { return; }

	// Set slide value back to default
	movement->GroundFriction = _fDefaultGroundFriction;
	movement->BrakingFrictionFactor = _fDefaultBrakingFrictionFactor;
	movement->BrakingDecelerationWalking = _fDefaultBrakingDecelerationWalking;
}

// Movement | Slide Jump ************************************************************************************************************************

/*
*
*/
void AArenaCharacter::InputSlideJump()
{
	if (_bSlideJumpEnabled && _bCanSlideJump && !IsTryingToVault() && _uStaminaComponents.Num() > 0)
	{
		// Get relevant stamina via matching channel (if required)
		UStamina* stamina = NULL;
		bool canSlideJump = true;
		if (_bSlideJumpRequiresStamina)
		{
			for (int i = 0; i < _uStaminaComponents.Num(); i++)
			{
				if (_uStaminaComponents[i]->GetStaminaChannel() == _iSlideJumpStaminaChannel)
				{
					stamina = _uStaminaComponents[i];
					break;
				}
			}
		}

		// Valid stamina channel found
		if (stamina != NULL) { canSlideJump = stamina->IsFullyRecharged(); }
		else
		{
			// Didn't find a valid stamina channel & we require one
			if (_bSlideJumpRequiresStamina)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
					TEXT("ERROR: Cannot < Dash > due to _uStaminaComponents not finding a valid matching stamina channel."));
				return;
			}
		}

		if (canSlideJump)
		{
			// Drain stamina
			if (stamina != NULL)
			{
				stamina->SetStamina(0.0f);
				stamina->DelayedRecharge();
			}

			// Slide jump
			_bCanSlideJump = false;
			FVector force = FVector(_fSlideJumpLaunchXForce, 0.0f, _fSlideJumpLaunchZForce);
			if (GetLocalRole() == ROLE_Authority)
			{
				LaunchCharacter(force, _fSlideJumpLaunchXYOverride, true);
				_bIsSlideJumping = true;
			} else
			{
				Server_Reliable_LaunchCharacter(force, _fSlideJumpLaunchXYOverride, _fSlideJumpLaunchZOverride);
				Server_Reliable_SetSlideJumping(true);
			}

			// Start landing checks
			_bIsPerformingGroundChecks = true;

			// Play feedback(s) [Camera shakes / Gamepad Rumbles]
			OwningClient_PlayCameraShake(_CameraShakeJumpStart, _fDoubleJumpCameraShakeScale);
			OwningClient_GamepadRumble(_fThrustGamepadRumbleIntensity, _fThrustGamepadRumbleDuration,
				_fThrustGamepadRumbleAffectsLeftLarge, _fThrustGamepadRumbleAffectsLeftSmall,
				_fThrustGamepadRumbleAffectsRightLarge, _fThrustGamepadRumbleAffectsRightSmall);

			// Stop sliding
			InputSlideExit();
			///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Slide Jump!"));
		}

	}
}

///////////////////////////////////////////////

/**
* @summary:	Sets the whether the character is slide jumping or not.
*
* @networking:	Runs on server
*
* @param:	bool SlideJumping
*
* @return:	void
*/
bool AArenaCharacter::Server_Reliable_SetSlideJumping_Validate(bool SlideJumping)
{ return true; }

void AArenaCharacter::Server_Reliable_SetSlideJumping_Implementation(bool SlideJumping)
{
	_bIsSlideJumping = SlideJumping;
}

// Movement | Wall Running **********************************************************************************************************************

/*
*
*/
void AArenaCharacter::OnRep_IsWallRunning()
{
	if (_bIsTryingToWallJump)
	{
		// Set _bIsJumping = TRUE
		if (GetLocalRole() == ROLE_Authority)
		{ _bIsJumping = true; } else
		{ Server_Reliable_SetJumping(true); }

		// Action jump
		FVector launchVelocity = GetWallRunLaunchVelocity();
		if (GetLocalRole() == ROLE_Authority)
		{ Multicast_Reliable_LaunchCharacter(launchVelocity, _bWallRunJumpXYOverride, true); }
		else
		{ Server_Reliable_LaunchCharacter(launchVelocity, _bWallRunJumpXYOverride, true); }

		///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, launchVelocity.ToString());

		// Play feedback(s) [Camera shakes / Gamepad Rumbles]
		OwningClient_PlayCameraShake(_CameraShakeJumpStart, 1.0f);
		OwningClient_GamepadRumble(_fJumpGamepadRumbleIntensity, _fJumpGamepadRumbleDuration,
			_fJumpGamepadRumbleAffectsLeftLarge, _fJumpGamepadRumbleAffectsLeftSmall,
			_fJumpGamepadRumbleAffectsRightLarge, _fJumpGamepadRumbleAffectsRightSmall);
	
		///_bIsTryingToWallJump = false;
	}
}

/*
*
*/
void AArenaCharacter::SetHorizontalVelocity(float HorizontalVelocityX, float HorizontalVelocityY)
{
	UCharacterMovementComponent* movementComponent = GetCharacterMovement();
	if (movementComponent == NULL) { return; }

	// Convert FVector2D velocity to FVector velocity
	FVector vec = FVector();
	vec.X = HorizontalVelocityX;
	vec.Y = HorizontalVelocityY;
	vec.Z = movementComponent->Velocity.Z;
	movementComponent->Velocity = vec;
}

/*
*
*/
bool AArenaCharacter::Server_Reliable_SetHorizontalVelocity_Validate(float HorizontalVelocityX, float HorizontalVelocityY) { return true; }

void AArenaCharacter::Server_Reliable_SetHorizontalVelocity_Implementation(float HorizontalVelocityX, float HorizontalVelocityY)
{
	SetHorizontalVelocity(HorizontalVelocityX, HorizontalVelocityY);
}

/*
*
*/
FVector2D AArenaCharacter::GetHorizontalVelocity() const
{
	UCharacterMovementComponent* movementComponent = GetCharacterMovement();
	if (movementComponent == NULL) { return FVector2D(); }

	// Convert FVector velocity to FVector2D velocity
	FVector2D vec = FVector2D();
	vec.X = movementComponent->Velocity.X;
	vec.Y = movementComponent->Velocity.Y;
	
	return FVector2D();
}

/*
*
*/
void AArenaCharacter::ClampHorizontalVelocity()
{
	UCharacterMovementComponent* movementComponent = GetCharacterMovement();
	if (movementComponent == NULL) { return; }

	if (movementComponent->IsFalling())
	{		
		float vecLength = UKismetMathLibrary::VSize2D(GetHorizontalVelocity());
		vecLength /= movementComponent->GetMaxSpeed();

		// Moving too fast?
		if (vecLength > 1.0)
		{
			FVector2D newVelocity = UKismetMathLibrary::Multiply_Vector2DFloat(GetHorizontalVelocity(), vecLength);
			
			// Apply new velocity
			if (GetLocalRole() == ROLE_Authority)
			{ SetHorizontalVelocity(newVelocity.X, newVelocity.Y); }
			else
			{ Server_Reliable_SetHorizontalVelocity(newVelocity.X, newVelocity.Y); }
		}
	}
}

/*
*
*/
FVector AArenaCharacter::DetermineRunDirectionAndSide(FVector WallNormal, E_WallRunDirection& WallRunDirectionEnum)
{
	FVector2D normal = FVector2D();
	normal.X = WallNormal.X;
	normal.Y = WallNormal.Y;

	FVector2D right = FVector2D();
	right.X = GetActorRightVector().X;
	right.Y = GetActorRightVector().Y;

	float dot = UKismetMathLibrary::DotProduct2D(normal, right);
	E_WallRunDirection eDir = E_WallRunDirection::eWRD_Up;
	if (dot > 0) { eDir = E_WallRunDirection::eWRD_Right; }
	else { eDir = E_WallRunDirection::eWRD_Left; }

	FVector select = FVector::ZeroVector;
	if (eDir == E_WallRunDirection::eWRD_Left) { select = FVector(0.0f, 0.0f, -1.0f); }
	else if (eDir == E_WallRunDirection::eWRD_Right) { select = FVector(0.0f, 0.0f, 1.0f); }
	///else { select = FVector(0.0f, 0.0f, 0.0f); }

	WallRunDirectionEnum = eDir;
	return UKismetMathLibrary::Cross_VectorVector(WallNormal, select);
}

/*
*
*/
bool AArenaCharacter::ValidWallRunInput() const
{
	// No valid forward input
	if (_fForwardInputScale < 0.1f) { return false; }
	if (_bIsTryingToWallJump) { return false; }

	bool validInput = false;
	switch (_eWallRunSide)
	{
	case E_WallRunDirection::eWRD_Left:
	{
		validInput = _fRightInputScale > 0.1f;
		break;
	}
	case E_WallRunDirection::eWRD_Right:
	{
		validInput = _fRightInputScale < -0.1f;
		break;
	}
	case E_WallRunDirection::eWRD_Up:
	{
		validInput = true;
		break;
	}
	default: break;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, validInput ? TEXT("true") : TEXT("false"));
	return validInput;
}

/*
*
*/
bool AArenaCharacter::CheckIfSurfaceCanBeWallRan(FVector SurfaceNormal) const
{
	UCharacterMovementComponent* movementComponent = GetCharacterMovement();
	if (movementComponent == NULL) { return false; }

	float z = SurfaceNormal.Z;
	if (z < -0.05) { return false; }

	FVector vec = FVector(SurfaceNormal.X, SurfaceNormal.Y, 0.0f);
	UKismetMathLibrary::Vector_Normalize(vec);
	float dot = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(vec, SurfaceNormal));
	return (dot < movementComponent->GetWalkableFloorAngle());
}

/*
*
*/
void AArenaCharacter::OnCapsuleComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UCharacterMovementComponent* movementComponent = GetCharacterMovement();
	if (movementComponent == NULL) { return; }
	
	// Not currently wall-running
	if (!_bIsWallRunning && Hit.IsValidBlockingHit())
	{
		// Valid wall-runnable surface & currently falling
		if (CheckIfSurfaceCanBeWallRan(Hit.ImpactNormal) && movementComponent->IsFalling())
		{
			E_WallRunDirection eDir = E_WallRunDirection::eWRD_Up;
			_WallRunDirection = DetermineRunDirectionAndSide(Hit.ImpactNormal, eDir);

			// Set wall run side/direction
			if (GetLocalRole() == ROLE_Authority) { _eWallRunSide = eDir; } 
			else { Server_Reliable_SetWallRunDirection(eDir); }
			
			// Start wall running (if there's valid input)
			if (ValidWallRunInput()) { StartWallRun(); }
		}
	}
}

/*
*
*/
FVector AArenaCharacter::GetWallRunLaunchVelocity()
{
	FVector launchDirection = FVector::ZeroVector;
	UCharacterMovementComponent* movementComponent = GetCharacterMovement();
	if (movementComponent == NULL) { return FVector::ZeroVector; }

	FVector v = FVector::ZeroVector;
	if (_eWallRunSide == E_WallRunDirection::eWRD_Left) { v = FVector(_fWallRunJumpLaunchXForce, (-1 * _fWallRunJumpLaunchYForce), _fWallRunJumpLaunchZForce); }
	else if (_eWallRunSide == E_WallRunDirection::eWRD_Right) { v = FVector(_fWallRunJumpLaunchXForce, _fWallRunJumpLaunchYForce, _fWallRunJumpLaunchZForce); }
	else { v = FVector((-1 * _fWallRunJumpLaunchXForce), 0.0f, _fWallRunJumpLaunchZForce); }

	float yaw = GetCharacterMovement()->GetPawnOwner()->GetControlRotation().Yaw;
	FQuat quat = FQuat::MakeFromEuler(FVector(0.0f, 0.0f, yaw));
	v  = UKismetMathLibrary::Quat_RotateVector(quat, v);
	return v;

	if (_bIsWallRunning)
	{
		FVector select = FVector::ZeroVector;
		if (_eWallRunSide == E_WallRunDirection::eWRD_Left) { select = FVector(0.0f, 0.0f, -1.0f); } 
		else if (_eWallRunSide == E_WallRunDirection::eWRD_Right) { select = FVector(0.0f, 0.0f, 1.0f); } 
		else { select = FVector(0.0f, 0.0f, 0.0f); }

		launchDirection = UKismetMathLibrary::Cross_VectorVector(_WallRunDirection, select);
	}
	launchDirection += FVector(0.0f, 0.0f, 1.0f);
	return UKismetMathLibrary::Multiply_VectorFloat(launchDirection, movementComponent->JumpZVelocity);
}

/*
*
*/
void AArenaCharacter::StartWallRun()
{
	UCharacterMovementComponent* movementComponent = GetCharacterMovement();
	if (movementComponent == NULL) { return; }

	movementComponent->AirControl = 1.0f;
	movementComponent->GravityScale = 0.0f;
	movementComponent->SetPlaneConstraintNormal(FVector(0.0f, 0.0f, 1.0f));

	if (GetLocalRole() == ROLE_Authority) { _bIsWallRunning = true; OnRep_IsWallRunning(); }
	else { Server_Reliable_SetIsWallRunning(true); }
	
	// Reset double jump
	if (_bDoubleJumpEnabled) { _bCanDoubleJump = true; }

	_bLerpWallRunCamera = true;
}

/*
*
*/
void AArenaCharacter::EndWallRun()
{
	UCharacterMovementComponent* movementComponent = GetCharacterMovement();
	if (movementComponent == NULL) { return; }

	movementComponent->AirControl = _fDefaultAirControl;
	movementComponent->GravityScale = _fDefaultGravityScale;
	movementComponent->SetPlaneConstraintNormal(FVector(0.0f, 0.0f, 0.0f));

	if (GetLocalRole() == ROLE_Authority) { _bIsWallRunning = false; OnRep_IsWallRunning(); }
	else { Server_Reliable_SetIsWallRunning(false); }

	// Get relevant stamina via matching channel
	UStamina* stamina = NULL;
	for (int i = 0; i < _uStaminaComponents.Num(); i++)
	{
		if (_uStaminaComponents[i]->GetStaminaChannel() == _iWallRunStaminaChannel)
		{
			stamina = _uStaminaComponents[i];
			break;
		}
	}

	if (stamina == NULL) { return; }
	else
	{
		// Found a matching stamina channel
		stamina->StopDrainingStamina();
		stamina->DelayedRecharge();
	}

	_bLerpWallRunCamera = false;
}

/*
*
*/
void AArenaCharacter::Tick_WallRunning()
{
	if (!_bWallRunEnabled) { EndWallRun(); }

	UCharacterMovementComponent* movementComponent = GetCharacterMovement();
	if (movementComponent == NULL) { return; }

	// Get relevant stamina via matching channel
	UStamina* stamina = NULL;
	for (int i = 0; i < _uStaminaComponents.Num(); i++)
	{
		if (_uStaminaComponents[i]->GetStaminaChannel() == _iWallRunStaminaChannel)
		{
			stamina = _uStaminaComponents[i];
			break;
		}
	}

	// Didn't find a matching stamina channel
	if (stamina == NULL) { return; }

	// Valid wall running input & theres valid stamina left
	if (ValidWallRunInput() && stamina->HasStamina())
	{
		// Trace to current wall-running side to ensure that there is still a valid wall to wall run on
		FVector select = FVector::ZeroVector;
		if (_eWallRunSide == E_WallRunDirection::eWRD_Left) { select = FVector(0.0f, 0.0f, -1.0f); }
		else if (_eWallRunSide == E_WallRunDirection::eWRD_Right) { select = FVector(0.0f, 0.0f, 1.0f); }
		///else { select = FVector(0.0f, 0.0f, 0.0f); }

		FVector cross = UKismetMathLibrary::Cross_VectorVector(_WallRunDirection, select);
		FVector endTrace = GetActorLocation() + cross * 200.0f;
		
		FHitResult hitResult;
		FCollisionQueryParams params;
		params.bTraceComplex = false;
		params.AddIgnoredActor(this);
		GetWorld()->LineTraceSingleByChannel(hitResult, GetActorLocation(), endTrace, ECollisionChannel::ECC_Visibility, params);
		
		if (hitResult.IsValidBlockingHit())
		{
			E_WallRunDirection eDir = E_WallRunDirection::eWRD_Up;
			FVector vDir = DetermineRunDirectionAndSide(hitResult.ImpactNormal, eDir);

			// Moving in the same assumed direction needed for the wall run (EG: We are moving forward relative to a left-sided wall run & not trying to move forward relative to a right-sided wall run)
			if (eDir == _eWallRunSide)
			{
				_WallRunDirection = vDir;
				///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::Printf(TEXT("vDir %s"), *vDir.ToString()));
				FVector v = UKismetMathLibrary::Multiply_VectorFloat(_WallRunDirection, movementComponent->GetMaxSpeed());
				movementComponent->Velocity = FVector(v.X, v.Y, 0.0f);

				// Drain stamina
				stamina->StartDrainingStamina();
			} 
			else { EndWallRun(); }
		} 
		else { EndWallRun(); }
	}
	else { EndWallRun(); }
}

/*
*
*/
bool AArenaCharacter::Server_Reliable_SetWallRunDirection_Validate(E_WallRunDirection WallRunDirection)
{ return true; }

void AArenaCharacter::Server_Reliable_SetWallRunDirection_Implementation(E_WallRunDirection WallRunDirection)
{
	_eWallRunSide = WallRunDirection;
}

/*
*
*/
bool AArenaCharacter::Server_Reliable_SetIsWallRunning_Validate(bool IsWallRunning) { return true; }

void AArenaCharacter::Server_Reliable_SetIsWallRunning_Implementation(bool IsWallRunning)
{
	_bIsWallRunning = IsWallRunning;
	if (GetLocalRole() == ROLE_Authority) { OnRep_IsWallRunning(); }
}