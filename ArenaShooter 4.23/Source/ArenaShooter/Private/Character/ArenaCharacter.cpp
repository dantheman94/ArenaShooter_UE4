// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaCharacter.h"

#include "FireMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
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
	bReplicateMovement = true;

	// Get default vars 
	_fDefaultAirControl = GetCharacterMovement()->AirControl;
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
	DOREPLIFETIME(AArenaCharacter, _eDashDirection);
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

	// Get all firing mode components as reference
	this->GetComponents<UStamina>(_uStaminaComponents);
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

	if (_bIsPerformingGroundChecks) { OnGroundChecks(); }
}

///////////////////////////////////////////////

void AArenaCharacter::OnGroundChecks()
{
	// Character has landed on the ground
	if (GetCharacterMovement()->IsMovingOnGround()) 
	{
		// Reset jumping
		Server_Reliable_SetJumping(false);
		Server_Reliable_SetDoubleJumping(false);		
		if (_bDoubleJumpEnabled) { _bCanDoubleJump = true; }

		// Reset falling/hover
		GetCharacterMovement()->AirControl = _fDefaultAirControl;
		_bCanHover = true;
		_bHoverCancelled = false;

		// Landing camera shake
		float shakeScale = UKismetMathLibrary::NormalizeToRange(_fFallingVelocity *= -1, 0.0f, 2000.0f);
		shakeScale *= 2;
		OwningClient_PlayCameraShake(_CameraShakeJumpLand, shakeScale);

		_bIsPerformingGroundChecks = false;
	}

	// Character is falling
	else { _fFallingVelocity = GetVelocity().Z; }
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
				stamina->DelayedRecharge(stamina->GetRechargeDelayTime());

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
	if (ctrl->IsInputKeyDown(EKeys::W))
	{	
		// Left
		if (ctrl->IsInputKeyDown(EKeys::A))
		{ dir = E_Direction::eGA_FwdL; }

		// Right
		else if (ctrl->IsInputKeyDown(EKeys::D))
		{ dir = E_Direction::eGA_FwdR; }

		// Center
		else
		{ dir = E_Direction::eGA_Fwd; }
	} 

	// Not forward
	else 
	{
		// Backward
		if (ctrl->IsInputKeyDown(EKeys::S))
		{
			// Left
			if (ctrl->IsInputKeyDown(EKeys::A))
			{ dir = E_Direction::eGA_BwdL; }

			// Right
			else if (ctrl->IsInputKeyDown(EKeys::D))
			{ dir = E_Direction::eGA_BwdR; }

			// Center
			else
			{ dir = E_Direction::eGA_Bwd; }
			
		}

		// Not backward
		else
		{
			// Left
			if (ctrl->IsInputKeyDown(EKeys::A))
			{ dir = E_Direction::eGA_Left; }

			// Right
			if (ctrl->IsInputKeyDown(EKeys::D))
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
	if (_bHoverEnabled && _bCanHover && !_bIsDuelWielding)
	{
		UCharacterMovementComponent* moveComp = GetCharacterMovement();
		if (moveComp == NULL) { return; }

		if (moveComp->IsFalling())
		{

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
		// Stop hovering
		Server_Reliable_SetHovering(false);
		Server_Reliable_ChangeHoverState(false);

		// Landing on ground checks [ updates on Tick() ]
		_bIsPerformingGroundChecks = true;
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
	if (IsHovering)
	{ Server_Reliable_SetMovementMode(MOVE_Falling); }

	// Not hovering
	else
	{
		UCharacterMovementComponent* moveComp = GetCharacterMovement();
		if (moveComp == NULL) { return; }

		// Falling or jumping
		if (moveComp->IsFalling())
		{ Server_Reliable_SetMovementMode(MOVE_Falling); }

		// Not falling
		else
		{ Server_Reliable_SetMovementMode(MOVE_Walking); }
	}
}

// Movement | Jump ************************************************************************************************************************

/*
*
*/
void AArenaCharacter::InputJump()
{
	if (_bCanJump)
	{
		// Sanity check
		if (GetCharacterMovement() == NULL) { return; } 
		else
		{
			// Not falling
			if (!GetCharacterMovement()->IsFalling())
			{
				// Uncrouch then jump
				if (_bIsCrouching) { ExitCrouch(); } else
				{
					// Jump
					Server_Reliable_SetJumping(true);
					Jump();

					// Start landing checks
					_bIsPerformingGroundChecks = true;

					// Gamepad rumble
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

			// Falling 
			else { InputDoubleJump(); }
		}
	} else { return; }
}

///////////////////////////////////////////////

/*
*
*/
void AArenaCharacter::InputDoubleJump()
{
	if (_bDoubleJumpEnabled && _bCanDoubleJump && _uStaminaComponents.Num() > 0)
	{
		// Get relevant stamina via matching channel
		UStamina* stamina = NULL;
		for (int i = 0; i < _uStaminaComponents.Num(); i++)
		{
			if (_uStaminaComponents[i]->GetStaminaChannel() == _iDoubleJumpStaminaChannel)
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
				stamina->DelayedRecharge(stamina->GetRechargeDelayTime());
				
				// Double jump
				_bCanDoubleJump = false;
				Server_Reliable_LaunchCharacter(FVector(0.0f, 0.0f, _fDoubleJumpForce), false, true);
				Server_Reliable_SetDoubleJumping(true);

				// Start landing checks
				_bIsPerformingGroundChecks = true;

				// Gamepad rumble
				OwningClient_GamepadRumble(_fThrustGamepadRumbleIntensity, _fThrustGamepadRumbleDuration,
					_fThrustGamepadRumbleAffectsLeftLarge, _fThrustGamepadRumbleAffectsLeftSmall,
					_fThrustGamepadRumbleAffectsRightLarge, _fThrustGamepadRumbleAffectsRightSmall);
			}
		}

		// stamina == NULL
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange,
				TEXT("WARNING: Cannot < Double Jump > due to _uStaminaComponents not finding a valid matching stamina channel."));
		}
	}
}

///////////////////////////////////////////////

#pragma region Server_Reliable_LaunchCharacter

/*
*
*/
bool AArenaCharacter::Server_Reliable_LaunchCharacter_Validate(FVector LaunchVelocity, bool XYOverride, bool ZOverride)
{ return true; }

void AArenaCharacter::Server_Reliable_LaunchCharacter_Implementation(FVector LaunchVelocity, bool XYOverride, bool ZOverride)
{
	Multicast_Reliable_LaunchCharacter(LaunchVelocity, XYOverride, ZOverride);
}

#pragma endregion

///////////////////////////////////////////////

#pragma region Multicast_Reliable_LaunchCharacter

/*
*
*/
bool AArenaCharacter::Multicast_Reliable_LaunchCharacter_Validate(FVector LaunchVelocity, bool XYOverride, bool ZOverride)
{ return true; }

void AArenaCharacter::Multicast_Reliable_LaunchCharacter_Implementation(FVector LaunchVelocity, bool XYOverride, bool ZOverride)
{
	LaunchCharacter(LaunchVelocity, XYOverride, ZOverride);
}

#pragma endregion

// Movement | Jump ************************************************************************************************************************

#pragma region Server_Reliable_SetDoubleJumping

bool AArenaCharacter::Server_Reliable_SetDoubleJumping_Validate(bool DoubleJumping)
{ return true; }

void AArenaCharacter::Server_Reliable_SetDoubleJumping_Implementation(bool DoubleJumping)
{
	_bIsDoubleJumping = DoubleJumping;
}

#pragma endregion