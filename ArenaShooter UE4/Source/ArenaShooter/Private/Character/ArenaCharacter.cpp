// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaCharacter.h"

#include "BasePlayerController.h"
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
#include "Structures.h"
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

	DOREPLIFETIME(AArenaCharacter, _bDashExiting);
	DOREPLIFETIME(AArenaCharacter, _bIsDashing);
	DOREPLIFETIME(AArenaCharacter, _bIsDoubleJumping);
	DOREPLIFETIME(AArenaCharacter, _bIsHovering);
	DOREPLIFETIME(AArenaCharacter, _bIsSliding);
	DOREPLIFETIME(AArenaCharacter, _bIsWallRunning);
	DOREPLIFETIME(AArenaCharacter, _eDashDirection);
	DOREPLIFETIME(AArenaCharacter, _eWallRunSide);
	DOREPLIFETIME(AArenaCharacter, _DashExitVelocityStart);
	DOREPLIFETIME(AArenaCharacter, _DashExitVelocityEnd);
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
	if (_bIsWallRunning) { Tick_WallRunning(DeltaTime); }

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
		if (controller != NULL)
		{
			float roll = controller->GetControlRotation().Roll;

			if (roll != 0)
			{
				if (roll >= 360 - _fWallRunningRollMaximum - 1) { AddControllerRollInput(_fWallRunningOriginAdditive); }
				if (roll <= _fWallRunningRollMaximum + 1) { AddControllerRollInput(-_fWallRunningOriginAdditive); }

				///GEngine->AddOnScreenDebugMessage(5, 5.0f, FColor::Cyan, FString::SanitizeFloat(controller->GetControlRotation().Roll));
			}
		}
	}

	// Ramp down velocity
	if (_bDashExiting)
	{
		if (_fVelocityZeroingTime < _fDashExitDuration)
		{
			// Zero the velocity over time
			_fVelocityZeroingTime += DeltaTime;

			// Set velocity
			FVector vLerp = UKismetMathLibrary::VLerp(_DashExitVelocityStart, _DashExitVelocityEnd, _fVelocityZeroingTime / _fDashExitDuration);
			///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, vLerp.ToString());
			GetCharacterMovement()->Velocity = vLerp;

			// Check if we're finished
			if (_fVelocityZeroingTime >= _fDashExitDuration)
			{
				if (GetLocalRole() == ROLE_Authority)
				{ _bDashExiting = false; }
				else
				{ Server_Reliable_SetDashExiting(false); }
			}
		}
	}
}

///////////////////////////////////////////////

/*
* @summary:	Called every frame (if _bIsWallRunning == true)
*
* @param:	DeltaTime	- The delta of the frame.
*
* @return:	void
*/
void AArenaCharacter::Tick_WallRunning(float DeltaTime)
{
	if (!_bWallRunEnabled) { EndWallRun(); }

	UCharacterMovementComponent* movementComponent = GetCharacterMovement();
	if (movementComponent == NULL) { return; }

	// Get relevant stamina via matching channel
	UStamina* stamina = NULL;
	bool canWallRun = true; // Represents if there's a valid stamina channel found if needed
	if (_bWallRunningRequiresStamina)
	{
		for (int i = 0; i < _uStaminaComponents.Num(); i++)
		{
			if (_uStaminaComponents[i]->GetStaminaChannel() == _iWallRunStaminaChannel)
			{
				stamina = _uStaminaComponents[i];
				break;
			}
		}
	}

	// Valid stamina channel found
	if (stamina != NULL) { canWallRun = stamina->HasStamina(); } else
	{
		// Didn't find a valid stamina channel & we require one
		if (_bWallRunningRequiresStamina)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
				TEXT("ERROR: Cannot < Wall Run > due to _uStaminaComponents not finding a valid matching stamina channel."));
			return;
		}
	}

	// Didn't find a matching stamina channel
	if (stamina == NULL && _bWallRunningRequiresStamina) { return; }

	if (canWallRun)
	{
		// Valid wall running input & theres valid stamina left
		bool hasStamina = true;
		if (stamina != NULL) { hasStamina = stamina->HasStamina(); } // This is so if there isn't a requirement for stamina to wall run then the game will assume that there is enough stamina
		if (ValidWallRunInput() && hasStamina)
		{
			// Trace to current wall-running side to ensure that there is still a valid wall to wall run on
			FVector select = FVector::ZeroVector;
			if (_eWallRunSide == E_WallRunDirection::eWRD_Left) { select = FVector(0.0f, 0.0f, -1.0f); } else if (_eWallRunSide == E_WallRunDirection::eWRD_Right) { select = FVector(0.0f, 0.0f, 1.0f); }
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
					if (stamina != NULL) { stamina->StartDrainingStamina(_bWallRunningCustomDrainRate ? _fWallRunStaminaDrainRate : 0.0f); }

					// Push the player down (to imitate a slow slipping action off the wall)
					FVector slip = FVector(0.0f, 0.0f, (_fWallRunningVerticalSlip * -1) * DeltaTime);
					LaunchCharacter(slip, false, false);

				} else { EndWallRun(); }
			} else { EndWallRun(); }
		} else { EndWallRun(); }
	} else { EndWallRun(); }
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
		if (_bIsTryingToSlide)
		{
			Slide(_bWasDashingWhenSlideInputWasPressed); 
			_bWasDashingWhenSlideInputWasPressed = false;
		}
		
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

// Movement | Grapple Hook ****************************************************************************************************************

/*
*
*/
void AArenaCharacter::GrappleHookEnter()
{
	if (!_bGrappleHookEnabled) { return; }

	_fOnGrappleHook.Broadcast();
}

/*
*
*/
void AArenaCharacter::GrappleHookExit()
{
	if (!_bGrappleHookEnabled) { return; }

	_fOnGrappleHookRelease.Broadcast();
}

// Movement | Dash ************************************************************************************************************************

/*
*
*/
void AArenaCharacter::InputDash()
{
	if (!_bDashEnabled) { return; }

	if (_bCanDash && _uStaminaComponents.Num() > 0)
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
			// Enough stamina to perform action?
			bool enoughStamina = false;
			if (!_bDashDrainsStaminaCompletely)
			{
				// Can perform multiple dashes per max stamina
				float currentStaminaPercentage = stamina->GetStamina() / stamina->GetMaxStaminaAmount();
				enoughStamina = currentStaminaPercentage >= _fDashStaminaPercentageCost;
			}
			
			// Dash requires full stamina
			else { enoughStamina = stamina->IsFullyRecharged(); }
			if (enoughStamina)
			{
				// Current stamina is max stamina
				float maxStamina = stamina->GetMaxStaminaAmount();

				// Determine amount to subtract from stamina
				float deductAmount = maxStamina * _fDashStaminaPercentageCost;

				// New stamina amount
				float newStamina = _bDashDrainsStaminaCompletely ? 0.0f : stamina->GetStamina() - deductAmount;
				stamina->SetStamina(newStamina);
				stamina->StopDrainingStamina();

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

				// Remove gravity
				if (HasAuthority())
				{ Multicast_Reliable_SetGravityScale(_fDashGravityScale); }
				else
				{ Server_Reliable_SetGravityScale(_fDashGravityScale); }

				// Dash
				*launchVector = FVector(launchVector->X, launchVector->Y, 0.0f);
				if (HasAuthority())
				{ Multicast_Reliable_LaunchCharacter(*launchVector, true, true); }
				else
				{ Server_Reliable_LaunchCharacter(*launchVector, true, true); }

				_bCanExitDash = true;
				
				// Start Dash timer
				Server_Reliable_SetDashing(true);
				FTimerDelegate dashDelegate;
				dashDelegate.BindUFunction(this, FName("StopDashing"), *launchVector);
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

/*

*/
FVector AArenaCharacter::DirectionalInputToVector()
{
	float yaw = GetCharacterMovement()->GetPawnOwner()->GetControlRotation().Yaw;
	FQuat quat = FQuat::MakeFromEuler(FVector(0.0f, 0.0f, yaw));
	
	// Get direction from input
	FVector launchVector = FVector(FVector::ZeroVector);
	E_Direction direction = GetDirectionFromInput();
	switch (direction)
	{

	// Idle
	case E_Direction::eGA_Idle:
	{		
		launchVector = UKismetMathLibrary::Quat_RotateVector(quat, FVector(0.0f, 0.0f, 0.0f));
		break;
	}

	// Forward
	case E_Direction::eGA_Fwd:
	{
		launchVector = UKismetMathLibrary::Quat_RotateVector(quat, FVector(1.0f, 0.0f, 0.0f));
		break;
	}

	// Forward left
	case E_Direction::eGA_FwdL:
	{
		launchVector = UKismetMathLibrary::Quat_RotateVector(quat, FVector(1.0f, -1.0f, 0.0f));
		break;
	}

	// Forward right
	case E_Direction::eGA_FwdR:
	{
		launchVector = UKismetMathLibrary::Quat_RotateVector(quat, FVector(1.0f, 1.0f, 0.0f));
		break;
	}

	// Backward
	case E_Direction::eGA_Bwd:
	{
		launchVector = UKismetMathLibrary::Quat_RotateVector(quat, FVector(-1.0f, 0.0f, 0.0f));
		break;
	}

	// Backword Left
	case E_Direction::eGA_BwdL:
	{
		launchVector = UKismetMathLibrary::Quat_RotateVector(quat, FVector(-1.0f, -1.0f, 0.0f));
		break;
	}

	// Backword Rigth
	case E_Direction::eGA_BwdR:
	{
		launchVector = UKismetMathLibrary::Quat_RotateVector(quat, FVector(-1.0f, 1.0f, 0.0f));
		break;
	}

	// Left
	case E_Direction::eGA_Left:
	{
		launchVector = UKismetMathLibrary::Quat_RotateVector(quat, FVector(0.0f, -1.0f, 0.0f));
		break;
	}

	// Right
	case E_Direction::eGA_Right:
	{
		launchVector = UKismetMathLibrary::Quat_RotateVector(quat, FVector(0.0f, 1.0f, 0.0f));
		break;
	}
	default: break;
	}

	return launchVector;
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
void AArenaCharacter::StopDashing(FVector LaunchVelocity)
{
	// Reset friction
	GetCharacterMovement()->GroundFriction = _fJogGroundFriction;

	Server_Reliable_SetDashing(false);

	// Set weapon firing capabilities
	_bCanFirePrimary = true;
	_bCanFireSecondary = _bIsDuelWielding; // Can only fire secondary weapon if we're currently duel-wielding

	// Stop velocity to zero so we don't launch for fkn ages
	_fVelocityZeroingTime = 0.0f;
	if (_bCanExitDash && _bSeparateDashExitSpeed)
	{
		FVector vec = UKismetMathLibrary::ClampVectorSize(LaunchVelocity, 0.0f, _fDashExitSpeed);
		///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, vec.ToString());
		if (GetLocalRole() == ROLE_Authority)
		{
			_bDashExiting = true;
			_DashExitVelocityStart = GetCharacterMovement()->Velocity;
			_DashExitVelocityEnd = vec;
		} else
		{
			Server_Reliable_SetDashExiting(true);
			Server_Reliable_SetDashExitingStartVelocity(GetCharacterMovement()->Velocity);
			Server_Reliable_SetDashExitingEndVelocity(vec);
		}
	}

	// Reset gravity
	if (HasAuthority())
	{ Multicast_Reliable_SetGravityScale(_fDefaultGravityScale); } 
	else
	{ Server_Reliable_SetGravityScale(_fDefaultGravityScale); }
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

///////////////////////////////////////////////

bool AArenaCharacter::Server_Reliable_SetDashExiting_Validate(bool DashExiting)
{ return true; }

void AArenaCharacter::Server_Reliable_SetDashExiting_Implementation(bool DashExiting)
{
	_bDashExiting = DashExiting;
}

///////////////////////////////////////////////

/*
*
*/
bool AArenaCharacter::Server_Reliable_SetDashExitingStartVelocity_Validate(FVector Velocity)
{ return true; }

void AArenaCharacter::Server_Reliable_SetDashExitingStartVelocity_Implementation(FVector Velocity)
{
	_DashExitVelocityStart = Velocity;
}

///////////////////////////////////////////////

/*
*
*/
bool AArenaCharacter::Server_Reliable_SetDashExitingEndVelocity_Validate(FVector Velocity)
{ return true; }

void AArenaCharacter::Server_Reliable_SetDashExitingEndVelocity_Implementation(FVector Velocity)
{
	_DashExitVelocityEnd = Velocity;
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
			// Get relevant stamina via matching channel
			UStamina* stamina = NULL;
			bool canHover = true; // Represents if there's a valid stamina channel found if needed
			if (_bHoverRequiresStamina)
			{
				for (int i = 0; i < _uStaminaComponents.Num(); i++)
				{
					if (_uStaminaComponents[i]->GetStaminaChannel() == _iHoverStaminaChannel)
					{
						stamina = _uStaminaComponents[i];
						break;
					}
				}
			}

			// Valid stamina channel found
			if (stamina != NULL) { canHover = stamina->HasStamina(); } 
			else
			{
				// Didn't find a valid stamina channel & we require one
				if (_bHoverRequiresStamina)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
						TEXT("ERROR: Cannot < Hover > due to _uStaminaComponents not finding a valid matching stamina channel."));
					return;
				}
			}

			if (canHover)
			{
				// Stop jumping if we are
				if (_bIsJumping)
				{
					if (GetLocalRole() == ROLE_Authority)
					{ _bIsJumping = false; } else
					{ Server_Reliable_SetJumping(false); }
				}

				// Stop double jumping if we are
				_bCanDoubleJump = false;
				if (_bIsDoubleJumping)
				{
					if (GetLocalRole() == ROLE_Authority)
					{ _bIsDoubleJumping = false; } else
					{ Server_Reliable_SetDoubleJumping(false); }
				}

				// Start hovering
				if (GetLocalRole() == ROLE_Authority)
				{
					_bIsHovering = true;
					Multicast_Reliable_ChangeHoverState(true);
				} else
				{
					Server_Reliable_SetHovering(true);
					Server_Reliable_ChangeHoverState(true);
				}

				// Cannot perform air control maneuverabilities
				moveComp->AirControl = 0.0f;

				_bCanHover = false; // This boolean represents whether we have hovered already (can only hover once per "falling" state, we have to touch the ground before we can hover again)
				_bHoverCancelled = false;

				FTimerDelegate hoverDelegate;
				hoverDelegate.BindUFunction(this, FName("HoverExit"));
				GetWorld()->GetTimerManager().SetTimer(_fHoverHandle, hoverDelegate, 1.0f, false, _fHoverDuration);

				// Drain stamina
				if (stamina != NULL) { stamina->StartDrainingStamina(_bHoverCustomDrainRate? _fHoverStaminaDrainRate : 0.0f); }
			}
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

		// Get relevant stamina via matching channel
		UStamina* stamina = NULL;
		if (_bHoverRequiresStamina)
		{
			for (int i = 0; i < _uStaminaComponents.Num(); i++)
			{
				if (_uStaminaComponents[i]->GetStaminaChannel() == _iHoverStaminaChannel)
				{
					stamina = _uStaminaComponents[i];
					break;
				}
			}
		}
		else { return; }

		// Stop draining stamina
		if (stamina != NULL) { stamina->StopDrainingStamina(); }
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
		if (stamina != NULL) { 

			// Determine if the current stamina value as a % is greater than the threshold needed to perform the action
			float currentStamina = stamina->GetStamina();
			float maxStamina = stamina->GetMaxStaminaAmount();
			float percent = currentStamina / maxStamina;
			canDoubleJump = percent > _fDoubleJumpStaminaCost;
			///canDoubleJump = stamina->IsFullyRecharged(); 
		}
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
			// Drain stamina (Completely)
			if (stamina != NULL)
			{
				// Stamina is full?
				if (stamina->IsFullyRecharged())
				{		
					// Determine amount to subtract from stamina
					float maxStamina = stamina->GetMaxStaminaAmount();
					float deductAmount = maxStamina * _fDoubleJumpStaminaCost;

					// New stamina amount
					float newStamina = _bDoubleJumpDrainsStaminaCompletely ? 0.0f : maxStamina - deductAmount;
					stamina->SetStamina(newStamina);
					stamina->StopDrainingStamina();
				}
			}

			// Can double jump
			_bCanDoubleJump = false;

			// Directional force (WASD)
			FVector force = DirectionalInputToVector();
			force *= _fDoubleJumpDirectionalForce;

			// Add vertical force to the directional force
			force += FVector(0.0f, 0.0f, _fDoubleJumpZForce);

			// Launch character
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

bool AArenaCharacter::Server_Reliable_SetDoubleJumping_Validate(bool DoubleJumping)
{ return true; }

void AArenaCharacter::Server_Reliable_SetDoubleJumping_Implementation(bool DoubleJumping)
{
	_bIsDoubleJumping = DoubleJumping;
}

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
		if (forwardSpeed > _fSlideForwardVelocityThreshold || forwardSpeed < -_fSlideForwardVelocityThreshold)
		{
			// If we're currently airbourne
			if (!GetCharacterMovement()->IsMovingOnGround())
			{
				// Fire trace to fall and check the distance of that trace
				GetCharacterMovement()->GravityScale = _fSlideAirbourneGravityForce;
				_bIsPerformingGroundChecks = true;
				_bIsTryingToSlide = true;
				_bWasDashingWhenSlideInputWasPressed = _bIsDashing;
			} 
			
			// Already touching the ground so just slide straight away
			else { Slide(_bIsDashing); }

			// Disable vaulting during a slide attempt
			_bCanVault = false;

			// This disables the velocity zeroing sequence once a dash maneuver is complete
			_bCanExitDash = false;
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

	// This re-enables the velocity zeroing sequence once a dash maneuver is complete
	_bCanExitDash = _bIsDashing;
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
void AArenaCharacter::Slide(bool WasDashing)
{
	// Can only slide when we're using forward/backward input
	APlayerController* playerController = Cast<APlayerController>(this->GetController());
	if (playerController == NULL) { return; }

	// If we're pressing forward, then our velocity HAS to be forward (and above threshold)
	// If we're pressing backward, then our velocity HAS to be backward (and under threshold)
	FVector velocity = GetCharacterMovement()->Velocity;
	float forwardSpeed = FVector::DotProduct(velocity, UKismetMathLibrary::GetForwardVector(GetActorRotation()));
	bool validForward = forwardSpeed > _fSlideForwardVelocityThreshold && playerController->IsInputKeyDown(EKeys::W);
	bool validBackward = forwardSpeed < -_fSlideForwardVelocityThreshold && playerController->IsInputKeyDown(EKeys::S);
	if (validForward || validBackward)
	{
		_bCanExitDash = false;

		// Reset gravity
		if (HasAuthority())
		{ Multicast_Reliable_SetGravityScale(_fDefaultGravityScale); } 
		else
		{ Server_Reliable_SetGravityScale(_fDefaultGravityScale); }

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
			if (GetLocalRole() != ROLE_Authority) { Multicast_Reliable_InitiateSlide(WasDashing); } else { Server_Reliable_InitiateSlide(WasDashing); }

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
bool AArenaCharacter::Server_Reliable_InitiateSlide_Validate(bool WasDashing)
{ return true; }

void AArenaCharacter::Server_Reliable_InitiateSlide_Implementation(bool WasDashing)
{
	Multicast_Reliable_InitiateSlide(WasDashing);
}

/*
*
*/
bool AArenaCharacter::Multicast_Reliable_InitiateSlide_Validate(bool WasDashing)
{ return true; }

void AArenaCharacter::Multicast_Reliable_InitiateSlide_Implementation(bool WasDashing)
{
	// Get movement component
	UCharacterMovementComponent* movement = GetCharacterMovement();
	if (movement == NULL) { return; }

	// Only slide launch if we're touching the ground (so airborne slides will wait till you hit the ground before launching you)
	if (movement->IsMovingOnGround())
	{
		// Set slide values
		movement->GroundFriction = 0.0f;
		movement->BrakingFrictionFactor = _fSlideBreakingFrictionFactor;
		movement->BrakingDecelerationWalking = _fSlideBrakingDeceleration;

		// Launch the character
		FVector forwardForce = FVector::ZeroVector;
		if (!WasDashing && _bOverrideSlideVelocityFromDash)
		{ forwardForce = GetActorForwardVector() * _fSlideForce; }
		else
		{ 
			float f = _DashExitVelocityEnd.Size() / _fSlideForce;
			float h = _fSlideForce * (f / 2);
			forwardForce = GetActorForwardVector() * (_fSlideForwardVelocityThreshold + h);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, FString::SanitizeFloat(_fSlideForwardVelocityThreshold + h));
		}
		FVector downForce = GetActorUpVector() * (_fSlideForce * -1.0f);
		FVector launchForce = forwardForce + downForce;
		this->LaunchCharacter(launchForce, /*_fSlideLaunchXYOverride &&*/ WasDashing, _fSlideLaunchZOverride);
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
				stamina->StopDrainingStamina();
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
	// We are currently exiting our wall run & are trying to jump
	if (_bIsTryingToWallJump)
	{
		// Get relevant stamina via matching channel
		UStamina* stamina = NULL;
		bool canWallRunJump = true;
		if (_bWallRunJumpingRequiresStamina)
		{
			for (int i = 0; i < _uStaminaComponents.Num(); i++)
			{
				if (_uStaminaComponents[i]->GetStaminaChannel() == _iWallRunJumpStaminaChannel)
				{
					stamina = _uStaminaComponents[i];
					break;
				}
			}
		}

		// Valid stamina channel found
		if (stamina != NULL) { canWallRunJump = stamina->IsFullyRecharged(); } 
		else
		{
			// Didn't find a valid stamina channel & we require one
			if (_bWallRunJumpingRequiresStamina)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
					TEXT("ERROR: Cannot < Double Jump > due to _uStaminaComponents not finding a valid matching stamina channel."));
				return;
			}
		}

		if (canWallRunJump) { WallRunJump(); }
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
	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (playerController == NULL) { return false; }

	// Get controller type (keyboard/gamepad)
	ABasePlayerController* basePlayerController = Cast<ABasePlayerController>(playerController);
	E_ControllerType controllerType = basePlayerController->GetControllerType();

	// Only need valid forward input if this is a keyboard/mouse combo
	if (controllerType == E_ControllerType::eCT_Keyboard)
	{
		if (_fForwardInputScale == 0.0f) { return false; }
	}
	
	// Cant wall run anymore if we're trying to jump off the wall
	if (_bIsTryingToWallJump) { return false; }

	// Checking for sideways input
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

	///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, validInput ? TEXT("true") : TEXT("false"));
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
void AArenaCharacter::StartWallRun()
{
	UCharacterMovementComponent* movementComponent = GetCharacterMovement();
	if (movementComponent == NULL) { return; }

	movementComponent->AirControl = 1.0f;
	movementComponent->GravityScale = 0.0f;
	movementComponent->SetPlaneConstraintNormal(FVector(0.0f, 0.0f, 1.0f));

	if (GetLocalRole() == ROLE_Authority) 
	{ 
		_bIsWallRunning = true; OnRep_IsWallRunning(); 		
		Server_Reliable_SetMovingSpeed(_fMovementSpeedWallRunning);
	}
	else 
	{ 
		Server_Reliable_SetIsWallRunning(true); 
		Server_Reliable_SetMovingSpeed(_fMovementSpeedWallRunning);
	}
	
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
	}

	_bLerpWallRunCamera = false;
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

// Movement | Wall Run Jump *********************************************************************************************************************

/*
*
*/
FVector AArenaCharacter::GetWallRunLaunchVelocity()
{
	FVector launchDirection = FVector::ZeroVector;
	UCharacterMovementComponent* movementComponent = GetCharacterMovement();
	if (movementComponent == NULL) { return FVector::ZeroVector; }

	FVector v = FVector::ZeroVector;
	if (_eWallRunSide == E_WallRunDirection::eWRD_Left)
	{ v = FVector(_fWallRunJumpLaunchXForce, (-1 * _fWallRunJumpLaunchYForce), _fWallRunJumpLaunchZForce); } 
	else if (_eWallRunSide == E_WallRunDirection::eWRD_Right)
	{ v = FVector(_fWallRunJumpLaunchXForce, _fWallRunJumpLaunchYForce, _fWallRunJumpLaunchZForce); } 
	else 
	{ v = FVector((-1 * _fWallRunJumpLaunchXForce), 0.0f, _fWallRunJumpLaunchZForce); }

	float yaw = GetCharacterMovement()->GetPawnOwner()->GetActorRotation().Yaw;
	FQuat quat = FQuat::MakeFromEuler(FVector(0.0f, 0.0f, yaw));
	v = UKismetMathLibrary::Quat_RotateVector(quat, v);
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
void AArenaCharacter::WallRunJump()
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
