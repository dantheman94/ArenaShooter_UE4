// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"

#include "Ammo.h"
#include "ArenaCharacter.h"
#include "BasePlayerController.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShake.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "FireMode.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interactable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Stamina.h"
#include "UnrealNetwork.h"
#include "Weapon.h"

// Startup ********************************************************************************************************************************

/**
* @summary:	Sets default values for this actor's properties.
*
* @return:	Constructor
*/
ABaseCharacter::ABaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Actor replicates
	bReplicates = true;
	bReplicateMovement = true;
	SetRemoteRoleForBackwardsCompat(ROLE_AutonomousProxy);

	// Bind damage events
	OnTakeAnyDamage.AddDynamic(this, &ABaseCharacter::OnAnyDamage);

#pragma region Create components

	// First person camera component
	USceneComponent* capsuleRoot = GetCapsuleComponent();
	_FirstPerson_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	_FirstPerson_Camera->SetupAttachment(capsuleRoot);
	_FirstPerson_Camera->SetRelativeLocationAndRotation(FVector(-35.0f, 0.0f, 75.0f), FRotator(0.0f, 0.0f, 0.0f));

	// First person spring arm component
	_FirstPerson_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FirstPersonSpringArm"));
	_FirstPerson_SpringArm->SetupAttachment(_FirstPerson_Camera);
	_FirstPerson_SpringArm->ProbeSize = 0.0f;
	_FirstPerson_SpringArm->bUsePawnControlRotation = true;
	_FirstPerson_SpringArm->bEnableCameraRotationLag;
	_FirstPerson_SpringArm->CameraRotationLagSpeed = 25.0f;

	// First person arms
	_FirstPerson_Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonArms"));
	_FirstPerson_Arms->SetupAttachment(_FirstPerson_SpringArm);
	_FirstPerson_Arms->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -167.5f), FRotator(0.0f, 0.0f, -90.0f));
	_FirstPerson_Arms->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_FirstPerson_Arms->SetOnlyOwnerSee(true);

	// First person primary weapon mesh
	_FirstPerson_PrimaryWeapon_SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonPrimaryWeaponMesh"));
	_FirstPerson_PrimaryWeapon_SkeletalMesh->SetupAttachment(_FirstPerson_Arms, "hand_r");
	_FirstPerson_PrimaryWeapon_SkeletalMesh->SetOnlyOwnerSee(true);
	_FirstPerson_PrimaryWeapon_SkeletalMesh->bSelfShadowOnly = true;
	_FirstPerson_PrimaryWeapon_SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// First person secondary weapon mesh
	_FirstPerson_SecondaryWeapon_SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonSecondaryWeaponMesh"));
	_FirstPerson_SecondaryWeapon_SkeletalMesh->SetupAttachment(_FirstPerson_Arms, "hand_l");
	_FirstPerson_SecondaryWeapon_SkeletalMesh->SetOnlyOwnerSee(true);
	_FirstPerson_SecondaryWeapon_SkeletalMesh->bSelfShadowOnly = true;
	_FirstPerson_SecondaryWeapon_SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Third person primary mesh
	_ThirdPerson_PrimaryWeapon_SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonPrimaryWeaponMesh"));
	_ThirdPerson_PrimaryWeapon_SkeletalMesh->SetupAttachment(GetMesh(), "hand_r");
	_ThirdPerson_PrimaryWeapon_SkeletalMesh->SetOwnerNoSee(true);
	_ThirdPerson_PrimaryWeapon_SkeletalMesh->bCastHiddenShadow = true;
	_ThirdPerson_PrimaryWeapon_SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Third person secondary mesh
	_ThirdPerson_SecondaryWeapon_SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonSecondaryWeaponMesh"));
	_ThirdPerson_SecondaryWeapon_SkeletalMesh->SetupAttachment(GetMesh(), "hand_l");
	_ThirdPerson_SecondaryWeapon_SkeletalMesh->SetOwnerNoSee(true);
	_ThirdPerson_SecondaryWeapon_SkeletalMesh->bCastHiddenShadow = true;
	_ThirdPerson_SecondaryWeapon_SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Third person reserve mesh
	_ThirdPerson_ReserveWeapon_SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonReserveWeaponMesh"));
	_ThirdPerson_ReserveWeapon_SkeletalMesh->SetupAttachment(GetMesh(), "spine_03");
	_ThirdPerson_ReserveWeapon_SkeletalMesh->SetRelativeLocationAndRotation(FVector(2.0f, -20.0f, 0.0f), FRotator(6.8f, -6.0f, -260.0f));
	_ThirdPerson_ReserveWeapon_SkeletalMesh->SetOwnerNoSee(true);
	_ThirdPerson_ReserveWeapon_SkeletalMesh->bCastHiddenShadow = true;
	_ThirdPerson_ReserveWeapon_SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

#pragma endregion

#pragma region Create hitboxes

	// Hitbox head
	_HitBox_Head = CreateDefaultSubobject<UCapsuleComponent>(TEXT("_Hitbox_Head"));
	_HitBox_Head->SetupAttachment(GetMesh(), "head");
	_HitBox_Head->SetRelativeLocationAndRotation(FVector(2.0f, -20.0f, 0.0f), FRotator(6.8f, -6.0f, -260.0f));
	_HitBox_Head->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_HitBox_Head->SetCollisionObjectType(ECC_GameTraceChannel9);

	// Hitbox torso upper
	_HitBox_TorsoUpper = CreateDefaultSubobject<UCapsuleComponent>(TEXT("_HitBox_TorsoUpper"));
	_HitBox_TorsoUpper->SetupAttachment(GetMesh(), "spine_03");
	_HitBox_TorsoUpper->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(-88.5f, 0.0f, 87.5f));
	_HitBox_TorsoUpper->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_HitBox_TorsoUpper->SetCollisionObjectType(ECC_GameTraceChannel9);

	// Hitbox torso lower
	_HitBox_TorsoLower = CreateDefaultSubobject<UCapsuleComponent>(TEXT("_HitBox_TorsoLower"));
	_HitBox_TorsoLower->SetupAttachment(GetMesh(), "spine_01");
	_HitBox_TorsoLower->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(-89.0f, 0.5f, 94.8f));
	_HitBox_TorsoLower->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_HitBox_TorsoLower->SetCollisionObjectType(ECC_GameTraceChannel9);

	// Hitbox right shoulder
	_HitBox_RightShoulder = CreateDefaultSubobject<UCapsuleComponent>(TEXT("_HitBox_RightShoulder"));
	_HitBox_RightShoulder->SetupAttachment(GetMesh(), "upperarm_r");
	_HitBox_RightShoulder->SetRelativeLocationAndRotation(FVector(-14.0f, 0.0f, -0.5f), FRotator(0.0f, -90.0f, 94.8f));
	_HitBox_RightShoulder->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_HitBox_RightShoulder->SetCollisionObjectType(ECC_GameTraceChannel9);

	// Hitbox right forearm
	_HitBox_RightForearm = CreateDefaultSubobject<UCapsuleComponent>(TEXT("_HitBox_RightForearm"));
	_HitBox_RightForearm->SetupAttachment(GetMesh(), "lowerarm_r");
	_HitBox_RightForearm->SetRelativeLocationAndRotation(FVector(-15.0f, 1.0f, -0.5f), FRotator(90.0f, 15.0f, -86.35f));
	_HitBox_RightForearm->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_HitBox_RightForearm->SetCollisionObjectType(ECC_GameTraceChannel9);

	// Hitbox right hand
	_HitBox_RightHand = CreateDefaultSubobject<UCapsuleComponent>(TEXT("_HitBox_RightHand"));
	_HitBox_RightHand->SetupAttachment(GetMesh(), "hand_r");
	_HitBox_RightHand->SetRelativeLocationAndRotation(FVector(-9.8f, 1.5f, -0.7f), FRotator(-166.0f, 81.5f, -180.0f));
	_HitBox_RightHand->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_HitBox_RightHand->SetCollisionObjectType(ECC_GameTraceChannel9);

	// Hitbox left shoulder
	_HitBox_LeftShoulder = CreateDefaultSubobject<UCapsuleComponent>(TEXT("_HitBox_LeftShoulder"));
	_HitBox_LeftShoulder->SetupAttachment(GetMesh(), "upperarm_l");
	_HitBox_LeftShoulder->SetRelativeLocationAndRotation(FVector(13.0f, 0.0f, 0.0f), FRotator(0.0f, -93.0f, 0.0f));
	_HitBox_LeftShoulder->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_HitBox_LeftShoulder->SetCollisionObjectType(ECC_GameTraceChannel9);

	// Hitbox left forearm
	_HitBox_LeftForearm = CreateDefaultSubobject<UCapsuleComponent>(TEXT("_HitBox_LeftForearm"));
	_HitBox_LeftForearm->SetupAttachment(GetMesh(), "lowerarm_l");
	_HitBox_LeftForearm->SetRelativeLocationAndRotation(FVector(12.35f, -1.0f, 0.6f), FRotator(-2.8f, -87.2f, 0.0f));
	_HitBox_LeftForearm->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_HitBox_LeftForearm->SetCollisionObjectType(ECC_GameTraceChannel9);

	// Hitbox left hand
	_HitBox_LeftHand = CreateDefaultSubobject<UCapsuleComponent>(TEXT("_HitBox_LeftHand"));
	_HitBox_LeftHand->SetupAttachment(GetMesh(), "hand_l");
	_HitBox_LeftHand->SetRelativeLocationAndRotation(FVector(6.8f, -1.7f, 0.25f), FRotator(0.0f, -81.5f, 0.0f));
	_HitBox_LeftHand->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_HitBox_LeftHand->SetCollisionObjectType(ECC_GameTraceChannel9);

	// Hitbox right thigh
	_HitBox_RightThigh = CreateDefaultSubobject<UCapsuleComponent>(TEXT("_HitBox_RightThigh"));
	_HitBox_RightThigh->SetupAttachment(GetMesh(), "thigh_r");
	_HitBox_RightThigh->SetRelativeLocationAndRotation(FVector(19.6f, -1.8f, 0.0f), FRotator(-268.6f, -0.15f, -88.5f));
	_HitBox_RightThigh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_HitBox_RightThigh->SetCollisionObjectType(ECC_GameTraceChannel9);

	// Hitbox right shin
	_HitBox_RightShin = CreateDefaultSubobject<UCapsuleComponent>(TEXT("_HitBox_RightShin"));
	_HitBox_RightShin->SetupAttachment(GetMesh(), "calf_r");
	_HitBox_RightShin->SetRelativeLocationAndRotation(FVector(22.35f, 0.0f, 1.0f), FRotator(-87.25f, 5.05f, -90.6f));
	_HitBox_RightShin->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_HitBox_RightShin->SetCollisionObjectType(ECC_GameTraceChannel9);

	// Hitbox left thigh
	_HitBox_LeftThigh = CreateDefaultSubobject<UCapsuleComponent>(TEXT("_HitBox_LeftThigh"));
	_HitBox_LeftThigh->SetupAttachment(GetMesh(), "thigh_l");
	_HitBox_LeftThigh->SetRelativeLocationAndRotation(FVector(-19.65f, 1.2f, -0.4f), FRotator(-180.0f, -90.0f, 180.0f));
	_HitBox_LeftThigh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_HitBox_LeftThigh->SetCollisionObjectType(ECC_GameTraceChannel9);

	// Hitbox left shin
	_HitBox_LeftShin = CreateDefaultSubobject<UCapsuleComponent>(TEXT("_HitBox_LeftShin"));
	_HitBox_LeftShin->SetupAttachment(GetMesh(), "calf_l");
	_HitBox_LeftShin->SetRelativeLocationAndRotation(FVector(-25.7f, -1.0f, -0.5f), FRotator(0.0f, 87.8f, 0.0f));
	_HitBox_LeftShin->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_HitBox_LeftShin->SetCollisionObjectType(ECC_GameTraceChannel9);

#pragma endregion

	// Get default variable values
	_fCameraRotationLagSpeed = _FirstPerson_SpringArm->CameraRotationLagSpeed;
	_fCapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

///////////////////////////////////////////////

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, _bIsAiming);
	DOREPLIFETIME(ABaseCharacter, _bIsCrouching);
	DOREPLIFETIME(ABaseCharacter, _bIsDuelWielding);
	DOREPLIFETIME(ABaseCharacter, _bIsEquippingNewWeapon);
	DOREPLIFETIME(ABaseCharacter, _bIsJumping);
	DOREPLIFETIME(ABaseCharacter, _bIsReloadingPrimaryWeapon);
	DOREPLIFETIME(ABaseCharacter, _bIsReloadingSecondaryWeapon);
	DOREPLIFETIME(ABaseCharacter, _bIsSprinting);
	DOREPLIFETIME(ABaseCharacter, _bIsTogglingWeapons);
	DOREPLIFETIME(ABaseCharacter, _bIsVaulting);
	DOREPLIFETIME(ABaseCharacter, _fForwardInputScale);
	DOREPLIFETIME(ABaseCharacter, _fRightInputScale);
	DOREPLIFETIME(ABaseCharacter, _iFragmentationGrenadeCount);
	DOREPLIFETIME(ABaseCharacter, _iEMPGrenadeCount);
	DOREPLIFETIME(ABaseCharacter, _iIncendiaryGrenadeCount);
	DOREPLIFETIME(ABaseCharacter, _PrimaryWeapon);
	DOREPLIFETIME(ABaseCharacter, _ReserveWeapon);
	DOREPLIFETIME(ABaseCharacter, _SecondaryWeapon);
}

///////////////////////////////////////////////

/**
* @summary:	Called when the game starts or when spawned.
*
* @return:	virtual void
*/
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get all stamina components as reference
	this->GetComponents<UStamina>(_uStaminaComponents);

	// Initialize starting loadout
	if (HasAuthority()) { Server_Reliable_CreateStartingLoadout(); }
}

// Animation ******************************************************************************************************************************

/*
*
*/
void ABaseCharacter::FreezeAnimation(UAnimMontage* MontageToFreeze, float EndFrame)
{
	UAnimInstance* animInstance = _FirstPerson_Arms->GetAnimInstance();
	if (animInstance == NULL) { return; }

	if (!_bCancelAnimation) { animInstance->Montage_Play(MontageToFreeze, 0.0f, EMontagePlayReturnType::MontageLength, EndFrame, true); }
	else { _bCancelAnimation = false; }
}

// Current Frame **************************************************************************************************************************

/**
* @summary:	Called every frame.
*
* @param:	DeltaTime	- The delta of the frame.
*
* @return:	virtual void
*/
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If we're taking damage, add to the boolean reset timer
	if (_bIsTakingDamage && _bTakingDamageResetTimerPosition <= _fTakingDamageResetDelay)
	{ _bTakingDamageResetTimerPosition += GetWorld()->GetDeltaSeconds(); } 
	else
	{
		// Reset timer & taking damage boolean to false
		_bTakingDamageResetTimerPosition = 0.0f;
		_bIsTakingDamage = false;
	}

	// Aim FOV lerping
	if (_bIsAimLerping && _PrimaryWeapon != NULL)
	{
		// Still lerping
		if (_fAimTime < _PrimaryWeapon->GetCurrentFireMode()->GetWeaponAimTime())
		{
			// Add to time
			_fAimTime += GetWorld()->GetDeltaSeconds();

			// Update FOV to target
			float alpha = _fAimTime / _PrimaryWeapon->GetCurrentFireMode()->GetWeaponAimTime();
			_FirstPerson_Camera->SetFieldOfView(FMath::Lerp(_fFovStart, _fFovTarget, alpha));
		}

		// Lerp complete
		else
		{
			if (_eAimDirection == E_AimDirection::ePT_ZoomIn)
			{ _iCurrentFovStage += 1; }
			else if (_eAimDirection == E_AimDirection::ePT_ZoomOut)
			{ _iCurrentFovStage = 0; }

			// Reset
			_bIsAimLerping = false;
			_fAimTime = 0.0f;
		}
	}
	
	// Trying to fire primary
	if (_bIsTryingToFirePrimary) { InitFirePrimaryWeapon(); }
	else
	{
		if (_PrimaryWeapon == NULL) { return; }
		if (_PrimaryWeapon->GetFireModes().Num() == 0) { return; }
		if (_PrimaryWeapon->GetCurrentFireMode()->IsFiring())
		{
			// Only setting to FALSE if _bIsFiring == true
			_PrimaryWeapon->GetCurrentFireMode()->SetIsFiring(false);
		}
	}

	// Trying to fire secondary

	// Reloading weapons
	if (_bIsReloadingPrimaryWeapon) { UpdateReloadingPrimary(); }
}

///////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::OnGroundChecks()
{
	// Character has landed on the ground
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		// Reset
		Server_Reliable_SetJumping(false);
		GetCharacterMovement()->AirControl = _fDefaultAirControl;

		// Landing camera shake
		float shakeScale = UKismetMathLibrary::NormalizeToRange(_fFallingVelocity *= -1, 0.0f, 2000.0f);
		shakeScale *= 2;
		OwningClient_PlayCameraShake(_CameraShakeJumpLand, shakeScale);

		_bIsPerformingGroundChecks = false;
	}

	// Character is falling
	else { _fFallingVelocity = GetVelocity().Z; }
}

///////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::UpdateReloadingPrimary()
{
	// Sanity checks
	if (_PrimaryWeapon == NULL) { return; }
	if (_PrimaryWeapon->GetFireModes().Num() == 0) { return; }
	if (_PrimaryWeapon->GetCurrentFireMode() == NULL) { return; }

	// Reload hasn't been canceled yet
	if (!_bPrimaryReloadCancelled)
	{
		// Ensure that _bIsReloadingPrimaryWeapon == true
		if (_bIsReloadingPrimaryWeapon == false) { Server_Reliable_SetIsReloadingPrimaryWeapon(true); }

		// Ensure that we cannot fire this weapon whilst it is still being reloaded
		_bCanFirePrimary = false;
	}

	// Reload canceled
	else
	{
		// Stop the reload timer
		FTimerHandle timerHandle = _PrimaryWeapon->GetCurrentFireMode()->GetReloadTimerHandle();
		GetWorldTimerManager().ClearTimer(timerHandle);

		// Can (try) to shoot weapon again
		_bCanFirePrimary = true;
		Server_Reliable_SetIsReloadingPrimaryWeapon(false);
		_bPrimaryReloadCancelled = false;

		// Stop montage
		UAnimInstance* animInstance = _FirstPerson_Arms->GetAnimInstance();
		if (animInstance == NULL) { return; }
		animInstance->StopAllMontages(0.0f);
	}

	// On reload complete
	if (_PrimaryWeapon->GetCurrentFireMode()->GetReloadComplete())
	{
		_bCanFirePrimary = true;
		Server_Reliable_SetIsReloadingPrimaryWeapon(false);
		_bPrimaryReloadCancelled = false;
	}
}

// Camera *********************************************************************************************************************************

bool ABaseCharacter::OwningClient_PlayCameraShake_Validate(TSubclassOf<class UCameraShake>, float Scale)
{ return GetController() != NULL; }

void ABaseCharacter::OwningClient_PlayCameraShake_Implementation(TSubclassOf<class UCameraShake> ShakeClass, float Scale)
{
	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (playerController != NULL)
	{
		playerController->PlayerCameraManager->PlayCameraShake(ShakeClass, Scale, ECameraAnimPlaySpace::CameraLocal, FRotator::ZeroRotator);
	}
}

// Controller | Gamepad *******************************************************************************************************************

bool ABaseCharacter::OwningClient_GamepadRumble_Validate(float Intensity, float Duration, 
	bool AffectsLeftLarge, bool AffectsLeftSmall, bool AffectsRightLarge, bool AffectsRightSmall)
{ return GetController() != NULL; }

void ABaseCharacter::OwningClient_GamepadRumble_Implementation(float Intensity, float Duration, 
	bool AffectsLeftLarge, bool AffectsLeftSmall, bool AffectsRightLarge, bool AffectsRightSmall)
{
	auto ctrl = Cast<ABasePlayerController>(GetController());
	ctrl->GamepadRumble(Intensity, Duration, AffectsLeftLarge, AffectsLeftSmall, AffectsRightLarge, AffectsRightSmall);
}

// Controller | Input *********************************************************************************************************************

/**
	* @summary:	Sets the whether the character is using controller rotation yaw or not.
	*
	* @networking:	Runs on server
	*
	* @param:	bool useControllerRotationYaw
	*
	* @return:	void
*/
bool ABaseCharacter::Server_SetUseControllerRotationYaw_Validate(bool useControllerRotationYaw)
{ return true; }

void ABaseCharacter::Server_SetUseControllerRotationYaw_Implementation(bool useControllerRotationYaw)
{
	if (HasAuthority()) { bUseControllerRotationYaw = useControllerRotationYaw; }
}

///////////////////////////////////////////////

void ABaseCharacter::SetForwardInputScale(float ForwardInputScale)
{
	if (Role == ROLE_Authority) 
	{ _fForwardInputScale = ForwardInputScale; }
	else 
	{ Server_SetForwardInputScale(ForwardInputScale); }
}

/**
* @summary:	Sets the whether forward input scale of the character (used for animation)
*
* @networking:	Runs on server
*
* @param:	float forwardInputScale
*
* @return:	void
*/
bool ABaseCharacter::Server_SetForwardInputScale_Validate(float ForwardInputScale)
{ return true; }

void ABaseCharacter::Server_SetForwardInputScale_Implementation(float ForwardInputScale)
{
	SetForwardInputScale(ForwardInputScale);
}

///////////////////////////////////////////////

void ABaseCharacter::SetRightInputScale(float RightInputScale)
{
	if (Role == ROLE_Authority) 
	{ _fRightInputScale = RightInputScale; }
	else 
	{ Server_SetRightInputScale(RightInputScale); }
}

/**
* @summary:	Sets the whether right input scale of the character (used for animation)
*
* @networking:	Runs on server
*
* @param:	float rightInputScale
*
* @return:	void
*/
bool ABaseCharacter::Server_SetRightInputScale_Validate(float RightInputScale)
{ return true; }

void ABaseCharacter::Server_SetRightInputScale_Implementation(float RightInputScale)
{
	SetRightInputScale(RightInputScale);
}

// First Person | Animation ***************************************************************************************************************

///////////////////////////////////////////////

/**
* @summary:	Plays the specific animation on the client's first person perspective
*
* @networking:	Runs on owning client
*
* @param:
* @param:
* @param:
* @param:
* @param:
* @param:
* @param:
* @param:
*
* @return:	void
*/
bool ABaseCharacter::OwningClient_PlayFirstPersonAnimation_Validate(float PlayRate, bool FreezeOnLastFrame,
	UAnimMontage* RightArmAnimation /*= NULL*/, float RightArmStartingFrame /*= 0.0f*/,
	UAnimMontage* LeftArmAnimation /*= NULL*/, float LeftArmStartingFrame /*= 0.0f*/,
	UAnimMontage* PrimaryGunAnimation /*= NULL*/, float PrimaryGunStartingFrame /*= 0.0f*/,
	UAnimMontage* SecondaryGunAnimation /*= NULL*/, float SecondaryGunStartingFrame /*= 0.0f*/)
{
	return RightArmAnimation != NULL || LeftArmAnimation != NULL || PrimaryGunAnimation != NULL || SecondaryGunAnimation != NULL;
}

void ABaseCharacter::OwningClient_PlayFirstPersonAnimation_Implementation(float PlayRate, bool FreezeOnLastFrame,
	UAnimMontage* RightArmAnimation /*= NULL*/, float RightArmStartingFrame /*= 0.0f*/,
	UAnimMontage* LeftArmAnimation /*= NULL*/, float LeftArmStartingFrame /*= 0.0f*/,
	UAnimMontage* PrimaryGunAnimation /*= NULL*/, float PrimaryGunStartingFrame /*= 0.0f*/,
	UAnimMontage* SecondaryGunAnimation /*= NULL*/, float SecondaryGunStartingFrame /*= 0.0f*/)
{
	// Play an animation on the right arm
	if (RightArmAnimation != NULL)
	{ _FirstPerson_Arms->GetAnimInstance()->Montage_Play(RightArmAnimation, PlayRate, EMontagePlayReturnType::MontageLength, RightArmStartingFrame, true); }

	// Play an animation on the left arm
	if (LeftArmAnimation != NULL)
	{ _FirstPerson_Arms->GetAnimInstance()->Montage_Play(LeftArmAnimation, PlayRate, EMontagePlayReturnType::MontageLength, LeftArmStartingFrame, true); }

	// Play an animation on the primary weapon
	if (PrimaryGunAnimation != NULL)
	{ _FirstPerson_PrimaryWeapon_SkeletalMesh->GetAnimInstance()->Montage_Play(PrimaryGunAnimation, PlayRate, EMontagePlayReturnType::MontageLength, PrimaryGunStartingFrame, true); }

	// Play an animation on the secondary weapon
	if (SecondaryGunAnimation != NULL)
	{ _FirstPerson_PrimaryWeapon_SkeletalMesh->GetAnimInstance()->Montage_Play(SecondaryGunAnimation, PlayRate, EMontagePlayReturnType::MontageLength, SecondaryGunStartingFrame, true); }
}

// Health | *******************************************************************************************************************************

void ABaseCharacter::OnAnyDamage(AActor* Actor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Role == ROLE_Authority)
	{
		// Is taking damage
		_bTakingDamageResetTimerPosition = 0.0f;
		_bIsTakingDamage = true;
		_fOnTakeDamage.Broadcast(Damage);

		if (GetCurrentShield() > 0.0f)
		{
			_fShield -= Damage;
		}
		else
		{
			_fFleshHealth -= Damage;
			
			// Clamp to zero
			if (_fFleshHealth < 0.0f) { _fFleshHealth = 0.0f; }
		}

		// Check if dead
		if (!IsAlive())
		{
		}
	}
	
	// Ensure that the method only runs server-side
	else
	{ Server_Reliable_OnAnyDamage(Actor, Damage, DamageType, InstigatedBy, DamageCauser); }
}

bool ABaseCharacter::Server_Reliable_OnAnyDamage_Validate(AActor* Actor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{ return true; }

void ABaseCharacter::Server_Reliable_OnAnyDamage_Implementation(AActor* Actor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	OnAnyDamage(Actor, Damage, DamageType, InstigatedBy, DamageCauser);
}

///////////////////////////////////////////////

void ABaseCharacter::OnPointDamage(float Damage)
{

}

///////////////////////////////////////////////

void ABaseCharacter::OnRadialDamage(float Damage)
{

}

void ABaseCharacter::OnDeath()
{
	if (Role == ROLE_Authority)
	{

	}

	// Ensure that the method only runs server-side
	else
	{ Server_Reliable_OnDeath(); }
}

bool ABaseCharacter::Server_Reliable_OnDeath_Validate()
{ return true; }

void ABaseCharacter::Server_Reliable_OnDeath_Implementation()
{
	OnDeath();
}

// Health | Burn **************************************************************************************************************************

bool ABaseCharacter::Server_Reliable_BurnCharacter_Validate(int Steps, float Damage, float Delay)
{ return true; }

void ABaseCharacter::Server_Reliable_BurnCharacter_Implementation(int Steps, float Damage, float Delay)
{
	// Only proceed if we're not being burned... 
	// (avoids overlapping calls since this is called whenever a projectile hits the character
	if (!_bIsBurning)
	{
		// Create a damage event  
		///TSubclassOf<UDamageType> const validDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());

		// Damage with initial burn
		///this->OnAnyDamage(this, Damage, validDamageTypeClass, NULL, NULL);

		// Do burn steps (We are doing one less burn step because of the initial burn before this)
		for (int i = 1; i < Steps; i++)
		{
			///FTimerDelegate burnDelegate;
			///burnDelegate.BindUFunction(this, FName("TakeDamage"), Damage);
			///GetWorld()->GetTimerManager().SetTimer(_fBurnDelayHandle, burnDelegate, Delay, false);
		}
	}
}

// Health | Flesh *************************************************************************************************************************

/**
* @summary:	Resets the flesh health of the character to the maximum amount.
*
* @networking:	Runs on server
*
* @return:	void
*/
bool ABaseCharacter::Server_Reliable_ResetFleshHealth_Validate()
{ return _fFleshHealth < _MAX_FLESH_HEALTH; }

void ABaseCharacter::Server_Reliable_ResetFleshHealth_Implementation()
{ _fFleshHealth = _MAX_FLESH_HEALTH; }

// Health | Shield ************************************************************************************************************************

/**
* @summary:	Resets the shield health of the character to the maximum amount.
*
* @networking:	Runs on server
*
* @return:	void
*/
bool ABaseCharacter::Server_Reliable_ResetShield_Validate()
{ return _fShield < _MAX_SHIELD; }

void ABaseCharacter::Server_Reliable_ResetShield_Implementation()
{ _fShield = _MAX_SHIELD; }

// Inventory | Starting *******************************************************************************************************************

bool ABaseCharacter::Server_Reliable_CreateStartingLoadout_Validate()
{ return true; }

void ABaseCharacter::Server_Reliable_CreateStartingLoadout_Implementation()
{
	if (_bUseStartingLoadoutFromGamemode)
	{

	}

	// _UseStartingLoadoutFromGamemode == false
	else 
	{
		// Spawn the weapon instances then assign them as the inventory weapons
		FVector location(0.0f, 0.0f, 0.0f);
		FRotator rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters spawnInfo;
		spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Primary (right hand)
		if (_StartingPrimaryWeaponClass)
		{
			// Spawn weapon & set
			AWeapon* weapon = GetWorld()->SpawnActor<AWeapon>(_StartingPrimaryWeaponClass, location, rotation, spawnInfo);
			Server_Reliable_SetPrimaryWeapon(weapon, true);
		}

		// Secondary (left hand)
		if (_StartingSecondaryWeaponClass)
		{
			// Spawn weapon & set
			AWeapon* weapon = GetWorld()->SpawnActor<AWeapon>(_StartingSecondaryWeaponClass, location, rotation, spawnInfo);
			Server_Reliable_SetSecondaryWeapon(weapon);
		}

		// Reserve (backpack) weapon
		if (_StartingReserveWeaponClass)
		{
			// Spawn weapon & set
			AWeapon* weapon = GetWorld()->SpawnActor<AWeapon>(_StartingReserveWeaponClass, location, rotation, spawnInfo);
			Server_Reliable_SetReserveWeapon(weapon);
		}
	}
}

// Inventory | Weapon *********************************************************************************************************************

/*
*
*/
void ABaseCharacter::FireTraceFullAuto()
{

}

///////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::InputToggleWeapon()
{
	// Sanity checks
	if (_PrimaryWeapon == NULL) { return; }
	if (_bIsTogglingWeapons == true) { return; }

	// Duel-wielding?
	if (_SecondaryWeapon != NULL)
	{
		// Cancel any reloads that may be happening
		_bSecondaryReloadCancelled = true;
	}
	else
	{
		// Only proceed if there is a reserve weapon to toggle with
		if (_ReserveWeapon == NULL) { return; }

		// Cancel any reloads that may be happening
		_bPrimaryReloadCancelled = true;		
		
		// Begin the toggle sequence
		Server_Reliable_SetupToggleWeapon();

		// Play lower animation
		uint8 handAnim = (uint8)E_HandAnimation::eHA_Unequip;
		UAnimMontage* lowerMontage = _PrimaryWeapon->GetArmAnimation((E_HandAnimation)handAnim);
		if (lowerMontage == NULL) { return; }
		OwningClient_PlayPrimaryWeaponFPAnimation(_fGlobalTogglePlayRate, true, true, handAnim, 0.0f, false, 0, 0.0f);
		float lowerDelay = lowerMontage->GetPlayLength() + 0.5f;

		// Start delay timer for the actual toggle
		FTimerDelegate toggleDelegate;
		toggleDelegate.BindUFunction(this, FName("Server_Reliable_ToggleWeapon"));
		GetWorld()->GetTimerManager().SetTimer(_fToggleHandle, toggleDelegate, 1.0f, false, lowerDelay);

		// Start delay timer for the weapon raise
		FTimerDelegate raiseDelegate;
		raiseDelegate.BindUFunction(this, FName("Server_Reliable_ExitToggleWeapon"));
		GetWorld()->GetTimerManager().SetTimer(_fExitToggleHandle, raiseDelegate, 1.0f, false, lowerDelay + 2.0f);
	}
}

///////////////////////////////////////////////

/*
*
*/
bool ABaseCharacter::Server_Reliable_SetupToggleWeapon_Validate()
{ return true; }

void ABaseCharacter::Server_Reliable_SetupToggleWeapon_Implementation()
{
	_bIsTogglingWeapons = true;
	
	// Set previous weapons
	_OldPrimaryWeapon = _PrimaryWeapon;
	_OldReserveWeapon = _ReserveWeapon;
}

///////////////////////////////////////////////

/*
*
*/
bool ABaseCharacter::Server_Reliable_ToggleWeapon_Validate()
{ return true; }

void ABaseCharacter::Server_Reliable_ToggleWeapon_Implementation()
{
	// Swap the weapons
	Server_Reliable_SetPrimaryWeapon(_OldReserveWeapon, false);
	Server_Reliable_SetReserveWeapon(_OldPrimaryWeapon);
}

///////////////////////////////////////////////

/*
*
*/
bool ABaseCharacter::Server_Reliable_ExitToggleWeapon_Validate()
{ return true; }

void ABaseCharacter::Server_Reliable_ExitToggleWeapon_Implementation()
{
	_bIsTogglingWeapons = false;

	// Play raise animation
	uint8 handAnim = (uint8)E_HandAnimation::eHA_Equip;
	OwningClient_PlayPrimaryWeaponFPAnimation(_fGlobalTogglePlayRate, false, true, handAnim, 0.0f, false, 0, 0.0f);
	UAnimMontage* raiseMontage = _PrimaryWeapon->GetArmAnimation((E_HandAnimation)handAnim);
	float raiseDelay = raiseMontage->GetPlayLength() + 0.5f;

}

// Inventory | Weapon | Aiming ************************************************************************************************************

/**
* @summary:	Sets the whether the character is aiming or not
*
* @param:	bool aiming
*
* @return:	void
*/
bool ABaseCharacter::Server_Reliable_SetIsAiming_Validate(bool aiming)
{ return true; }

void ABaseCharacter::Server_Reliable_SetIsAiming_Implementation(bool aiming)
{
	if (HasAuthority()) { _bIsAiming = aiming; }
}

///////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::AimWeaponEnter()
{
	// Character sanity checks
	if (_bIsReloadingPrimaryWeapon || _bIsReloadingSecondaryWeapon || _bIsTogglingWeapons) { return; }
	///if (_bIsSprinting) { SprintExit(); }
	if (!_bCanAim) { return; }

	// Weapon sanity checks
	if (_PrimaryWeapon == NULL) { return; }
	if (_PrimaryWeapon->GetCurrentFireMode() == NULL) { return; }
	if (_PrimaryWeapon->GetCurrentFireMode()->GetFovArray().Num() <= _iCurrentFovStage) { return; }

	// Reset
	_iCurrentFovStage = 0;

	// Disable camera boom
	_FirstPerson_SpringArm->bEnableCameraRotationLag = false;
	_FirstPerson_SpringArm->CameraRotationLagSpeed = 0.0f;
	FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	_FirstPerson_Arms->AttachToComponent(_FirstPerson_Camera, rules);

	// Set moving speed
	Server_Reliable_SetMovingSpeed(_fMovementSpeedWalk);

	// Set aiming
	_PrimaryWeapon->Server_Reliable_SetPawnOwnerIsAiming(true);
	if (HasAuthority()) { _bIsAiming = true; }
	else { Server_Reliable_SetIsAiming(true); }

	// Init lerp targets
	_fFovStart = _FirstPerson_Camera->FieldOfView;
	_fFovTarget = _PrimaryWeapon->GetCurrentFireMode()->GetFovArray()[0];
	
	_eAimDirection = E_AimDirection::ePT_ZoomIn;
	_bIsAimLerping = true;

	// Transform origin lerping
	if (_PrimaryWeapon->GetCurrentFireMode()->IsAimDownSightEnabled()) { _fAdsAnimationEvent.Broadcast(true); }
}

///////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::AimWeaponExit()
{
	if (_bIsAiming)
	{
		// Weapon sanity checks*
		if (_PrimaryWeapon == NULL) { return; }

		// Enable camera boom
		_FirstPerson_SpringArm->bEnableCameraRotationLag = true;
		_FirstPerson_SpringArm->CameraRotationLagSpeed = _fCameraRotationLagSpeed;
		FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
		_FirstPerson_Arms->AttachToComponent(_FirstPerson_SpringArm, rules);

		// Set moving speed
		Server_Reliable_SetMovingSpeed(_fMovementSpeedJog);

		// Set aiming
		_PrimaryWeapon->Server_Reliable_SetPawnOwnerIsAiming(false);
		Server_Reliable_SetIsAiming(false);

		// Init lerp targets
		_fFovStart = _FirstPerson_Camera->FieldOfView;
		_fFovTarget = Cast<ABasePlayerController>(GetController())->GetDefaultFov();

		_eAimDirection = E_AimDirection::ePT_ZoomOut;
		_bIsAimLerping = true;

		// Transform origin lerping
		if (_PrimaryWeapon->GetCurrentFireMode()->IsAimDownSightEnabled()) { _fAdsAnimationEvent.Broadcast(false); }
	}
}

// Inventory | Weapon | Primary ***********************************************************************************************************

/**
* @summary:	Sets the character's primary weapon
*
* @networking:	Runs on server
*
* @param:	AWeapon* Weapon
*
* @return:	void
*/
bool ABaseCharacter::Server_Reliable_SetPrimaryWeapon_Validate(AWeapon* Weapon, bool FirstPickup)
{ return true; }

void ABaseCharacter::Server_Reliable_SetPrimaryWeapon_Implementation(AWeapon* Weapon, bool FirstPickup)
{
	_PrimaryWeapon = Weapon;

	// Set weapon owner
	Weapon->SetOwner(this);
	Weapon->Server_Reliable_SetNewOwner(this);
	Weapon->Server_Reliable_SetOwnersPrimaryWeapon(true);

	// Update meshes
	///OwningClient_UpdateFirstPersonPrimaryWeaponMesh(Weapon, FirstPickup);
	///Multicast_UpdateThirdPersonPrimaryWeaponMesh(Weapon);
	if (Role == ROLE_Authority) { OnRep_PrimaryWeapon(); }
}

///////////////////////////////////////////////

bool ABaseCharacter::Multicast_UpdateThirdPersonPrimaryWeaponMesh_Validate(AWeapon* Weapon)
{ return true; }

void ABaseCharacter::Multicast_UpdateThirdPersonPrimaryWeaponMesh_Implementation(AWeapon* Weapon)
{
	// Either update the mesh with the mesh referenced from the weapon, otherwise clear the mesh on the character
	if (Weapon)
	{
		// Set mesh
		_ThirdPerson_PrimaryWeapon_SkeletalMesh->SetSkeletalMesh(Weapon->GetThirdPersonMesh());

		// Set animation instance (anim BP)
		GetMesh()->SetAnimInstanceClass(Weapon->GetAnimInstanceThirdPersonCharacter());
	}

	// Weapon == NULL
	else { _ThirdPerson_PrimaryWeapon_SkeletalMesh->SetSkeletalMesh(NULL); }
}

///////////////////////////////////////////////

bool ABaseCharacter::OwningClient_UpdateFirstPersonPrimaryWeaponMesh_Validate(AWeapon* Weapon, bool FirstPickup)
{ return true; }

void ABaseCharacter::OwningClient_UpdateFirstPersonPrimaryWeaponMesh_Implementation(AWeapon* Weapon, bool FirstPickup)
{
	// Either update the mesh with the mesh referenced from the weapon, otherwise clear the mesh on the character
	if (Weapon != NULL)
	{
		_FirstPerson_PrimaryWeapon_SkeletalMesh->SetSkeletalMesh(Weapon->GetFirstPersonMesh());

		if (_bIsDuelWielding)
		{
			// Update origin transforms
			FTransform t = FTransform::Identity;
			_FirstPerson_Arms->SetRelativeTransform(t);
			_FirstPerson_Arms->SetHiddenInGame(true);
			_FirstPerson_PrimaryWeapon_SkeletalMesh->SetRelativeTransform(Weapon->GetTransformOriginDuelPrimary());

			// Reset origin starting location in arena character (used for first person dash animation)
			AArenaCharacter* arenaChar = Cast<AArenaCharacter>(this);
			if (arenaChar) { arenaChar->SetFPRelativeStartingTransform(t); }

		} else // _bIsDuelWielding == FALSE
		{
			// Update origin transforms
			_FirstPerson_Arms->SetRelativeTransform(Weapon->GetTransformOriginHands());
			_FirstPerson_Arms->SetHiddenInGame(false);
			_FirstPerson_PrimaryWeapon_SkeletalMesh->SetRelativeTransform(Weapon->GetTransformOriginGun());

			// Reset origin starting location in arena character (used for first person dash animation)
			AArenaCharacter* arenaChar = Cast<AArenaCharacter>(this);
			if (arenaChar) { arenaChar->SetFPRelativeStartingTransform(Weapon->GetTransformOriginHands()); }

			// Set animation instance (anim BP)
			_FirstPerson_Arms->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			_FirstPerson_Arms->SetAnimInstanceClass(Weapon->GetAnimInstanceFirstPersonHands());
			_FirstPerson_PrimaryWeapon_SkeletalMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			_FirstPerson_PrimaryWeapon_SkeletalMesh->SetAnimInstanceClass(Weapon->GetAnimInstanceFirstPersonGun());

			///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("UPDATING PRIMARY FIRST PERSON ORIGIN"));
		}

		// Play animation
		if (FirstPickup)
		{
			uint8 handAnim = (uint8)E_HandAnimation::eHA_FirstPickup;
			uint8 gunAnim = (uint8)E_GunAnimation::eGA_FirstPickup;
			OwningClient_PlayPrimaryWeaponFPAnimation(1.0f, false, true, handAnim, 0.0f, true, gunAnim, 0.0f);
		} else
		{
			uint8 handAnim = (uint8)E_HandAnimation::eHA_Equip;
			uint8 gunAnim = (uint8)0;
			OwningClient_PlayPrimaryWeaponFPAnimation(1.0f, false, true, handAnim, 0.0f, false, gunAnim, 0.0f);
		}

	} else
	{
		// Weapon == NULL
		_FirstPerson_PrimaryWeapon_SkeletalMesh->SetAnimInstanceClass(NULL);
		_FirstPerson_PrimaryWeapon_SkeletalMesh->SetSkeletalMesh(NULL);
	}
}

///////////////////////////////////////////////

void ABaseCharacter::InitFirePrimaryWeapon()
{
	// Sanity check
	if (_PrimaryWeapon == NULL){ return; }
	if (_PrimaryWeapon->IsThereValidFireModeCount() == false) { return; }	
	if (_PrimaryWeapon->GetCurrentFireMode() == NULL) { return; }

	if (_bIsReloadingPrimaryWeapon) { return; }
	if (_bIsTogglingWeapons) { return; }
	if (_bIsSprinting) { return; }

	// Weapon requires a full charge to fire
	if (_PrimaryWeapon->GetCurrentFireMode()->GetWeaponMustBeChargedToFire())
	{
		
	}

	// Weapon does NOT require charge to fire
	else
	{
		// Fire delay complete?
		if (_PrimaryWeapon->GetCurrentFireMode()->FireDelayComplete())
		{
			// Fire
			OwningClient_Reliable_PrimaryWeaponCameraTrace();
		}
	}
}

///////////////////////////////////////////////

bool ABaseCharacter::OwningClient_Reliable_PrimaryWeaponCameraTrace_Validate()
{ return true; }

void ABaseCharacter::OwningClient_Reliable_PrimaryWeaponCameraTrace_Implementation()
{
	if (_PrimaryWeapon == NULL) { return; }

	// Get player controller ref
	ABasePlayerController* ctrl = Cast<ABasePlayerController>(GetController());

	// Get crosshair location in viewport (for now its just in the center of the screen, but will need some offset in the future)
	int viewportX, viewportY;
	ctrl->GetViewportSize(viewportX, viewportY);

	// Convert screen location to world space
	FVector worldLocation = FVector::ZeroVector;
	FVector worldDirection = FVector::ZeroVector;
	ctrl->DeprojectScreenPositionToWorld(viewportX / 2, viewportY / 2, worldLocation, worldDirection);

	// Get trace end point
	float traceLength = _PrimaryWeapon->GetCurrentFireMode()->GetMaxRangeThreshold();
	FVector spreadPoint = FMath::VRandCone(worldDirection, (_PrimaryWeapon->GetCurrentFireMode()->GetCurrentProjectileSpread() * GetWorld()->GetDeltaSeconds()) / 2);
	FVector traceEnd = FVector::ZeroVector;
	traceEnd = worldLocation + (worldDirection + (spreadPoint * traceLength));

	// Fire line trace
	FHitResult hitResult;
	FCollisionQueryParams params;
	params.bTraceComplex = false;
	params.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(hitResult, worldLocation, traceEnd, ECollisionChannel::ECC_Camera, params);
	///DrawDebugLine(GetWorld(), worldLocation, traceEnd, FColor::Red, true, 1.0f);

	Server_Reliable_PrimaryWeaponCameraTrace(hitResult);
}

///////////////////////////////////////////////

bool ABaseCharacter::Server_Reliable_PrimaryWeaponCameraTrace_Validate(FHitResult ClientHitResult)
{ return true; }

void ABaseCharacter::Server_Reliable_PrimaryWeaponCameraTrace_Implementation(FHitResult ClientHitResult)
{
	if (_FirstPerson_PrimaryWeapon_SkeletalMesh == NULL) { return; }

	if (ClientHitResult.bBlockingHit)
	{
		// Get projectile transform
		FTransform trans = FTransform::Identity;
		FVector charMuzzle = _FirstPerson_PrimaryWeapon_SkeletalMesh->GetSocketLocation("MuzzleLaunchPoint");
		FVector v = FVector(ClientHitResult.ImpactPoint - charMuzzle);
		UKismetMathLibrary::Vector_Normalize(v);
		FRotator rot = UKismetMathLibrary::MakeRotFromX(v);
		FQuat quat = FQuat(rot);
		trans.SetLocation(charMuzzle);
		trans.SetRotation(quat);

		// Start primary weapon fire
		_PrimaryWeapon->GetCurrentFireMode()->Fire(ClientHitResult, trans, _FirstPerson_PrimaryWeapon_SkeletalMesh, _ThirdPerson_PrimaryWeapon_SkeletalMesh);
	}

	// !ClientHitResult.bBlockingHit
	else
	{
		// Get projectile transform
		FTransform trans = FTransform::Identity;
		FVector charMuzzle = _FirstPerson_PrimaryWeapon_SkeletalMesh->GetSocketLocation("MuzzleLaunchPoint");
		FVector v = FVector(ClientHitResult.TraceEnd - charMuzzle);
		UKismetMathLibrary::Vector_Normalize(v);
		FRotator rot = UKismetMathLibrary::MakeRotFromX(v);
		FQuat quat = FQuat(rot);
		trans.SetLocation(charMuzzle);
		trans.SetRotation(quat);

		// Start primary weapon fire
		_PrimaryWeapon->GetCurrentFireMode()->Fire(ClientHitResult, trans, _FirstPerson_PrimaryWeapon_SkeletalMesh, _ThirdPerson_PrimaryWeapon_SkeletalMesh);
	}
}

///////////////////////////////////////////////

/**
* @summary:	Checks and initiates a reload of the character's primary weapon.
*
* @return:	virtual void
*/
void ABaseCharacter::InputReloadPrimaryWeapon()
{
	// Sanity check
	if (_PrimaryWeapon == NULL) { return; }
	if (!_PrimaryWeapon->IsThereValidFireModeCount()) { return; }
	if (_PrimaryWeapon->GetCurrentFireMode() == NULL) { return; }

	// Don't continue if we're already reloading
	if (_bIsReloadingPrimaryWeapon) { return; }

	_bPrimaryReloadCancelled = false;
	UFireMode* fireMode = _PrimaryWeapon->GetCurrentFireMode();
	if (fireMode == NULL) { return; }
	UAmmo* ammoPool = fireMode->GetAmmoPool();
	if (ammoPool == NULL) { return; }

	// Not duel wielding
	if (!_bIsDuelWielding)
	{
		switch (fireMode->GetReloadType())
		{
		case E_ReloadType::eRT_Magazine:
		{
			// Doesnt have ammo in reserves? Dont progress
			if (ammoPool->GetReserveAmmo() <= 0) { return; }

			// Is a round NOT in the chamber?
			if (!ammoPool->IsRoundInChamber())
			{
				// Full magazine?
				if (ammoPool->GetMagazineAmmo() >= ammoPool->GetMaximumMagazineAmmo())
				{
					// Chamber a round into the magazine (if available)
					if (ammoPool->GetMagazineAmmo() > 0)
					{
						// Play animation
						uint8 handAnimByte = (uint8)E_HandAnimation::eHA_ReloadFullEmpty;
						uint8 gunAnimByte = (uint8)E_GunAnimation::eGA_ReloadFullEmpty;
						float startingFrame = fireMode->GetReloadStartingTimeChamberRound();
						OwningClient_PlayPrimaryWeaponFPAnimation(_fGlobalReloadPlayRate, false, true, handAnimByte, startingFrame, true, gunAnimByte, startingFrame);

						return;
					}
				}
			}

			// Magazine full? Don't progress
			if (ammoPool->GetMagazineAmmo() >= ammoPool->GetMaximumMagazineAmmo()) { return; }

			// Cannot shoot whilst we are reloading
			_bCanFirePrimary = false;

			// Stop aiming
			AimWeaponExit();
			
			// Get reload montage reference (hands)
			uint8 handAnimByte;
			if (ammoPool->GetMagazineAmmo() >= 0 && ammoPool->IsRoundInChamber())
			{ handAnimByte = (uint8)E_HandAnimation::eHA_ReloadFullNotEmpty; } 
			else
			{ handAnimByte = (uint8)E_HandAnimation::eHA_ReloadFullEmpty; }

			// Get reload montage reference (weapon)
			uint8 gunAnimByte;
			if (ammoPool->GetMagazineAmmo() >= 0 && ammoPool->IsRoundInChamber())
			{ gunAnimByte = (uint8)E_GunAnimation::eGA_ReloadFullNotEmpty; } 
			else
			{ gunAnimByte = (uint8)E_GunAnimation::eGA_ReloadFullEmpty; }

			// Play reload animation
			float startingFrame = _PrimaryWeapon->GetCurrentFireMode()->GetReloadStartingTime();
			OwningClient_PlayPrimaryWeaponFPAnimation(_fGlobalReloadPlayRate, false, true, handAnimByte, startingFrame, true, gunAnimByte, startingFrame);

			Server_Reliable_SetIsReloadingPrimaryWeapon(true);

			break;
		}
		case E_ReloadType::eRT_LoadSingle:
		{
			break;
		}
		default: break;
		}
	}
	
	// Is duel wielding
	else
	{

	}

}

///////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::OnRep_PrimaryWeapon()
{
	// Update meshes
	OwningClient_UpdateFirstPersonPrimaryWeaponMesh(_PrimaryWeapon, false);
	Multicast_UpdateThirdPersonPrimaryWeaponMesh(_PrimaryWeapon);
}

///////////////////////////////////////////////

void ABaseCharacter::InputPrimaryFirePress()
{
	// Sanity check
	if (_PrimaryWeapon == NULL) { return; }
	if (_PrimaryWeapon->IsThereValidFireModeCount() == false) { return; }
	if (_PrimaryWeapon->GetCurrentFireMode() == NULL) { return; }
	
	// Only true if the trigger type is press enter
	if (_PrimaryWeapon->GetCurrentFireMode()->GetFiringTriggerType() == E_FiringTriggerType::eFTT_Press)
	{ _bIsTryingToFirePrimary = true; }
	else 
	{ _bIsTryingToFirePrimary = false; }
}

///////////////////////////////////////////////

void ABaseCharacter::InputPrimaryFireRelease()
{
	// Sanity check
	if (_PrimaryWeapon == NULL) { return; }
	if (_PrimaryWeapon->IsThereValidFireModeCount() == false) { return; }
	if (_PrimaryWeapon->GetCurrentFireMode() == NULL) { return; }

	// Only true if the trigger type is press exit
	if (_PrimaryWeapon->GetCurrentFireMode()->GetFiringTriggerType() == E_FiringTriggerType::eFTT_Release)
	{ _bIsTryingToFirePrimary = true; }
	else
	{ _bIsTryingToFirePrimary = false; }
}

///////////////////////////////////////////////

bool ABaseCharacter::OwningClient_PlayPrimaryWeaponFPAnimation_Validate(float PlayRate, bool FreezeMontageAtLastFrame,
	bool PlayHandAnimation, uint8 HandAnimation, float HandAnimationStartingFrame,
	bool PlayGunAnimation, uint8 GunAnimation, float GunAnimationStartingFrame)
{ return true; }

/*
*
*/
void ABaseCharacter::OwningClient_PlayPrimaryWeaponFPAnimation_Implementation(float PlayRate, bool FreezeMontageAtLastFrame,
	bool PlayHandAnimation, uint8 HandAnimation, float HandAnimationStartingFrame,
	bool PlayGunAnimation, uint8 GunAnimation, float GunAnimationStartingFrame)
{
	// Sanity checks
	if (_PrimaryWeapon == NULL) { return; }

	// Hand animation
	float i = 0.05f;
	if (PlayHandAnimation)
	{
		UAnimMontage* handMontage = _PrimaryWeapon->GetArmAnimation((E_HandAnimation)HandAnimation);
		UAnimInstance* handAnimInstance = _FirstPerson_Arms->GetAnimInstance();
		if (handMontage != NULL && handAnimInstance != NULL)
		{
			// Play hand montage
			handAnimInstance->Montage_Play(handMontage, PlayRate, EMontagePlayReturnType::MontageLength, HandAnimationStartingFrame, true);
			if (FreezeMontageAtLastFrame)
			{
				// Start delay timer then freeze anim
				float playLength = handMontage->GetPlayLength();
				FTimerDelegate animationDelegate;
				animationDelegate.BindUFunction(this, FName("FreezeAnimation"), handMontage, playLength);
				GetWorld()->GetTimerManager().SetTimer(_fPrimaryFPAnimationHandle, animationDelegate, 1.0f, false, (playLength / _fGlobalReloadPlayRate) - i);
			}
		}
	}

	// Gun animation
	if (PlayGunAnimation)
	{
		UAnimMontage* gunMontage = _PrimaryWeapon->GetGunAnimation((E_GunAnimation)GunAnimation);
		UAnimInstance* handAnimInstance = _FirstPerson_Arms->GetAnimInstance();
		if (gunMontage != NULL && handAnimInstance != NULL)
		{
			// Play gun montage
			handAnimInstance->Montage_Play(gunMontage, PlayRate, EMontagePlayReturnType::MontageLength, GunAnimationStartingFrame, true);
			if (FreezeMontageAtLastFrame)
			{
				// Start delay timer then freeze anim
				float playLength = gunMontage->GetPlayLength();
				FTimerDelegate animationDelegate;
				animationDelegate.BindUFunction(this, FName("FreezeAnimation"), gunMontage, playLength);
				GetWorld()->GetTimerManager().SetTimer(_fPrimaryFPAnimationHandle, animationDelegate, 1.0f, false, (playLength / _fGlobalReloadPlayRate) - i);
			}
		}
	}
}

///////////////////////////////////////////////

bool ABaseCharacter::Server_Reliable_SetIsReloadingPrimaryWeapon_Validate(bool ReloadingPrimary)
{ return true; }

void ABaseCharacter::Server_Reliable_SetIsReloadingPrimaryWeapon_Implementation(bool ReloadingPrimary)
{
	_bIsReloadingPrimaryWeapon = ReloadingPrimary;
}

// Inventory | Weapon | Secondary *********************************************************************************************************

/**
* @summary:	Sets the character's secondary weapon
*
* @networking:	Runs on server
*
* @param:	AWeapon* Weapon
*
* @return:	void
*/
bool ABaseCharacter::Server_Reliable_SetSecondaryWeapon_Validate(AWeapon* Weapon)
{ return Weapon != NULL; }

void ABaseCharacter::Server_Reliable_SetSecondaryWeapon_Implementation(AWeapon* Weapon)
{
	_SecondaryWeapon = Weapon;
}

///////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::InputFireSecondaryWeapon()
{

}

///////////////////////////////////////////////

/**
* @summary:	Checks and initiates a reload of the character's secondary weapon.
*
* @return:	virtual void
*/
void ABaseCharacter::InputReloadSecondaryWeapon()
{
	if (_bIsReloadingSecondaryWeapon) { return; }
}

// Inventory | Weapon | Reserve ***********************************************************************************************************

/**
* @summary:	Sets the character's reserve weapon
*
* @networking:	Runs on server
*
* @param:	AWeapon* Weapon
*
* @return:	void
*/
bool ABaseCharacter::Server_Reliable_SetReserveWeapon_Validate(AWeapon* Weapon)
{ return Weapon != NULL; }

void ABaseCharacter::Server_Reliable_SetReserveWeapon_Implementation(AWeapon* Weapon)
{
	_ReserveWeapon = Weapon;

	// Set weapon owner
	_ReserveWeapon->SetOwner(this);
	_ReserveWeapon->Server_Reliable_SetNewOwner(this);
	_ReserveWeapon->Server_Reliable_SetOwnersPrimaryWeapon(false);
	_ReserveWeapon->Server_Reliable_SetOwnersSecondaryWeapon(false);

	if (Role == ROLE_Authority) { OnRep_ReserveWeapon(); }
}

/*
*
*/
void ABaseCharacter::OnRep_ReserveWeapon()
{

}

// Interaction ****************************************************************************************************************************

/*
*
*/
AInteractable* ABaseCharacter::CalculateFocusInteractable()
{
	///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("CALCULATING") + FString::FromInt(_Interactables.Num()));
	AInteractable* currentFocus = NULL;
	if (_Interactables.Num() > 0)
	{
		// Cycle through all interactables and determine which is the closest
		float closestDistance = TNumericLimits<float>::Max();
		for (int i = 0; i < _Interactables.Num(); i++)
		{
			float dist = FVector::DistSquared(this->GetActorLocation(), _Interactables[i]->GetActorLocation());
			if (dist < closestDistance)
			{
				closestDistance = dist;
				currentFocus = _Interactables[i];
				///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("NAME: ") + currentFocus->GetInteractablePickupName().ToString());
			}
		}
	}

	_FocusInteractable = currentFocus;
	return _FocusInteractable;
}

///////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::AddToInteractablesArray(AInteractable* Interactable)
{
	_Interactables.Add(Interactable);
}

///////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::RemoveFromInteractablesArray(AInteractable* Interactable)
{
	if (_Interactables.Contains(Interactable)) { _Interactables.Remove(Interactable); }
}

///////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::InteractPrimary()
{
	// Sanity check
	if (_FocusInteractable == NULL) { return; }

	// Start interaction timer
	FTimerDelegate interactionDelegate;
	interactionDelegate.BindUFunction(this, FName("Interact"), false);
	GetWorld()->GetTimerManager().SetTimer(_fInteractionHandle, interactionDelegate, 1.0f, false, _fInteractionThresholdTime);
}

///////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::InteractSecondary()
{

}

///////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::CancelInteraction()
{
	// Cancel interaction timer
	GetWorld()->GetTimerManager().ClearTimer(_fInteractionHandle);
}

///////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::Interact(bool IsSecondary)
{
	// Sanity checks
	if (_FocusInteractable == NULL) { return; }
	if (_FocusInteractable->GetOnUsedActor() == NULL) { return; }

	// Interacting with a weapon?
	TSubclassOf<AWeapon> weaponClass = StaticCast<TSubclassOf<AWeapon>>(_FocusInteractable->GetOnUsedActor());
	if (weaponClass)
	{
		// Set new weapon
		int magSize = 100;
		int reserveSize = 100;
		int batterySize = 100;
		Server_Reliable_SpawnWeapon(IsSecondary, weaponClass, magSize, reserveSize, batterySize);

		// Drop old weapon
		///if (_PrimaryWeapon != NULL) { Server_Reliable_DropWeapon(_PrimaryWeapon); }
	}

	// Interacting with a NON-Weapon object
	else
	{

	}

	// On interact event plays on the interactable object (basically just destroy/re-pool)
	_FocusInteractable->Server_Reliable_OnInteract(this);
}

///////////////////////////////////////////////

/*
*
*/
bool ABaseCharacter::Server_Reliable_DropWeapon_Validate(AWeapon* WeaponInstance)
{ return true; }

void ABaseCharacter::Server_Reliable_DropWeapon_Implementation(AWeapon* WeaponInstance)
{
	// Get class
	TSubclassOf<AActor> actorClass = WeaponInstance->GetOnDroppedActor();
	if (actorClass == NULL) { return; }

	// Spawn info
	ECollisionChannel collisionChannel = ECollisionChannel::ECC_Camera;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);
	FActorSpawnParameters spawnInfo;
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn location
	FVector forwardVector = GetActorForwardVector() * 100.0f;
	FVector loc = GetCapsuleComponent()->GetComponentLocation();
	FVector eyeLocation = FVector(loc.X, loc.Y, loc.Z + BaseEyeHeight);
	   
	// Spawn dropped actor
	DrawDebugLine(GetWorld(), eyeLocation, eyeLocation + forwardVector, FColor::Red, true, 20.0f);
	AActor* droppedActor = GetWorld()->SpawnActor<AActor>(actorClass, eyeLocation + forwardVector, GetActorRotation(), spawnInfo);
}

///////////////////////////////////////////////

/*
*
*/
bool ABaseCharacter::Server_Reliable_SpawnWeapon_Validate(bool IsSecondary, TSubclassOf<AWeapon> WeaponClass, int MagazineSize, int ReserveSize, int BatterySize)
{ return true; }

void ABaseCharacter::Server_Reliable_SpawnWeapon_Implementation(bool IsSecondary, TSubclassOf<AWeapon> WeaponClass, int MagazineSize, int ReserveSize, int BatterySize)
{
	// Spawn info
	FVector location(0.0f, 0.0f, 0.0f);
	FRotator rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters spawnInfo;
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn weapon
	AWeapon* weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, location, rotation, spawnInfo);

	if (!IsSecondary)
	{ 
		Server_Reliable_DropWeapon(_PrimaryWeapon);
		Server_Reliable_SetPrimaryWeapon(weapon, true);
	}
	else 
	{
		Server_Reliable_DropWeapon(_SecondaryWeapon);
		Server_Reliable_SetSecondaryWeapon(weapon);
	}
}

// Movement | Base ************************************************************************************************************************

/**
* @summary:	Moves the character on the vertical axis (forward/backward)
*
* @return:	void
*/
void ABaseCharacter::MoveForward(float Value)
{
	if (Controller != NULL)
	{
		// Used for animation
		if (_fForwardInputScale != Value) { Server_SetForwardInputScale(Value); }

		if (Value != 0.0f)
		{
			// Determine which direction is forward
			FRotator rotation = Controller->GetControlRotation();

			// Limit pitch when walking or falling
			if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) { rotation.Pitch = 0.0f; }

			// Add movement in the forward direction
			const FVector direction = FRotationMatrix(rotation).GetScaledAxis(EAxis::X);
			AddMovementInput(direction, Value);
		} 

		// (Value == 0.0f)
		else
		{
			if (_fForwardInputScale == 0.0f && _fRightInputScale == 0.0f)
			{
				if (bUseControllerRotationYaw)
				{
					if (HasAuthority())
					{ bUseControllerRotationYaw = false; }
					else 
					{ Server_SetUseControllerRotationYaw(false); }
				}
			} else
			{
				if (!bUseControllerRotationYaw)
				{
					if (HasAuthority())
					{ bUseControllerRotationYaw = true; }
					else
					{ Server_SetUseControllerRotationYaw(true); }
				}
			}
		}
	}
}

///////////////////////////////////////////////

/**
* @summary:	Moves the character on the horizontal axis (left/right)
*
* @return:	void
*/
void ABaseCharacter::MoveRight(float Value)
{
	if (Controller != NULL)
	{
		// Used for animation
		if (_fRightInputScale != Value) { Server_SetRightInputScale(Value); }

		if (Value != 0.0f)
		{
			// Determine which direction is right
			FRotator rotation = Controller->GetControlRotation();
			const FVector direction = FRotationMatrix(rotation).GetScaledAxis(EAxis::Y);

			// Add movement in the right direction
			AddMovementInput(direction, Value);

		} else // (value == 0.0f)
		{
			if (_fForwardInputScale == 0.0f && _fRightInputScale == 0.0f)
			{
				if (bUseControllerRotationYaw)
				{
					if (HasAuthority()) { bUseControllerRotationYaw = false; } else { Server_SetUseControllerRotationYaw(false); }
				}
			} else
			{
				if (!bUseControllerRotationYaw)
				{
					if (HasAuthority()) { bUseControllerRotationYaw = true; } else { Server_SetUseControllerRotationYaw(true); }
				}
			}
		}
	}
}

///////////////////////////////////////////////

bool ABaseCharacter::Server_SetMovementMode_Validate(EMovementMode MovementMode)
{ return true; }

void ABaseCharacter::Server_SetMovementMode_Implementation(EMovementMode MovementMode)
{
	Multicast_SetMovementMode(MovementMode);
}

bool ABaseCharacter::Multicast_SetMovementMode_Validate(EMovementMode MovementMode)
{ return true; }

void ABaseCharacter::Multicast_SetMovementMode_Implementation(EMovementMode MovementMode)
{
	GetCharacterMovement()->SetMovementMode(MovementMode);
}

///////////////////////////////////////////////

bool ABaseCharacter::Server_Reliable_SetMovingSpeed_Validate(float Speed)
{ return GetCharacterMovement() != NULL; }

void ABaseCharacter::Server_Reliable_SetMovingSpeed_Implementation(float Speed)
{ Multicast_Reliable_SetMovingSpeed(Speed); }

///////////////////////////////////////////////

bool ABaseCharacter::Multicast_Reliable_SetMovingSpeed_Validate(float Speed)
{ return GetCharacterMovement() != NULL; }

void ABaseCharacter::Multicast_Reliable_SetMovingSpeed_Implementation(float Speed)
{ GetCharacterMovement()->MaxWalkSpeed = Speed; }

// Movement | Crouch **********************************************************************************************************************

void ABaseCharacter::CrouchToggle(bool Crouch)
{
	// Crouch enter
	if (Crouch)
	{
		// When initiating a crouch from a sprint state
		if (_bIsSprinting)
		{

		}

		// _bIsSprinting == FALSE
		else
		{ EnterCrouch(); }
	}

	// Crouch exit
	else
	{ ExitCrouch(); }
}

///////////////////////////////////////////////

void ABaseCharacter::EnterCrouch()
{
	// Cannot crouch when airborne
	if (!GetCharacterMovement()->IsMovingOnGround()) { return; }

	// Lerp camera transform?
	if (_PrimaryWeapon != NULL)
	{
		if (_PrimaryWeapon->GetCurrentFireMode()->IsAimDownSightEnabled())
		{ _bLerpCrouchCamera = !IsAiming(); } else
		{ _bLerpCrouchCamera = true; }
	}

	// _PrimaryWeapon == NULL
	else
	{ _bLerpCrouchCamera = true; }

	// Set _bIsCrouching = TRUE
	if (Role == ROLE_Authority)
	{ _bIsCrouching = true; } else
	{ Server_Reliable_SetIsCrouching(true); }
}

///////////////////////////////////////////////

void ABaseCharacter::ExitCrouch()
{
	// Can only stop crouching if we we're previously crouching TRUE????
	if (_bIsCrouching)
	{
		// Set _bIsCrouching = FALSE
		if (Role == ROLE_Authority)
		{ _bIsCrouching = false; } else
		{ Server_Reliable_SetIsCrouching(false); }
	}
}

///////////////////////////////////////////////

bool ABaseCharacter::Server_Reliable_SetIsCrouching_Validate(bool IsCrouching)
{ return true; }

void ABaseCharacter::Server_Reliable_SetIsCrouching_Implementation(bool IsCrouching)
{
	_bIsCrouching = IsCrouching;
}

// Movement | Jump ************************************************************************************************************************

void ABaseCharacter::InputJump()
{
	InputVault();

	if (_bCanJump && !_bIsTryingToVault)
	{
		// Sanity check
		if (GetCharacterMovement() == NULL) { return; } else
		{
			// Not falling
			if (!GetCharacterMovement()->IsFalling())
			{
				// Uncrouch then jump
				if (_bIsCrouching) { ExitCrouch(); } else
				{
					// Set _bIsJumping = TRUE
					if (Role == ROLE_Authority)
					{ _bIsJumping = true; } else
					{ Server_Reliable_SetJumping(true); }

					// Action jump
					Jump();

					// Play feedback(s) [Camera shakes / Gamepad Rumbles]
					OwningClient_PlayCameraShake(_CameraShakeJumpStart, 1.0f);
					OwningClient_GamepadRumble(_fJumpGamepadRumbleIntensity, _fJumpGamepadRumbleDuration,
						_fJumpGamepadRumbleAffectsLeftLarge, _fJumpGamepadRumbleAffectsLeftSmall,
						_fJumpGamepadRumbleAffectsRightLarge, _fJumpGamepadRumbleAffectsRightSmall);

					_bIsPerformingGroundChecks = true;
				}
			}
		}
	} else { return; }
}

///////////////////////////////////////////////

bool ABaseCharacter::Server_Reliable_SetJumping_Validate(bool Jumping)
{ return true; }

void ABaseCharacter::Server_Reliable_SetJumping_Implementation(bool Jumping)
{
	_bIsJumping = Jumping;
}

// Movement | Sprint **********************************************************************************************************************

/**
* @summary:	Sets the whether the character is using sprinting or not.
*
* @networking:	Runs on server
*
* @param:	bool sprinting
*
* @return:	void
*/
bool ABaseCharacter::Server_Reliable_SetSprinting_Validate(bool Sprinting)
{ return true; }

void ABaseCharacter::Server_Reliable_SetSprinting_Implementation(bool Sprinting)
{
	if (HasAuthority()) { _bIsSprinting = Sprinting; }
}

// Stamina ********************************************************************************************************************************

/**
* @summary:	Returns reference to the of stamina component attached to this character, specified by the channel.
*
* @param:	int Channel
*
* @return:	TArray<UStamina*>
*/
UStamina* ABaseCharacter::GetStaminaComponentByChannel(int Channel)
{
	// Get relevant stamina via matching channel
	UStamina* stamina = NULL;
	for (int i = 0; i < _uStaminaComponents.Num(); i++)
	{
		if (_uStaminaComponents[i]->GetStaminaChannel() == Channel)
		{
			stamina = _uStaminaComponents[i];
			break;
		}
	}
	return stamina;
}

// Movement | Vault ***********************************************************************************************************************

/*
*
*/
void ABaseCharacter::InputVault()
{
	// Reset
	_vWallHeightLocation = FVector::ZeroVector;
	_vWallImpactPoint = FVector::ZeroVector;
	_vWallNormal = FVector::ZeroVector;
	_vWallTraceEnd = FVector::ZeroVector;
	_vWallTraceStart = FVector::ZeroVector;

	// Forward trace
	FHitResult forwardHit = LedgeForwardTrace();
	_vWallImpactPoint = forwardHit.ImpactPoint;
	_vWallNormal = forwardHit.ImpactNormal;
	_vWallTraceStart = forwardHit.TraceStart;
	_vWallTraceEnd = forwardHit.TraceEnd;

	// Height trace
	FHitResult heightHit = LedgeHeightTrace();
	_vWallHeightLocation = heightHit.ImpactPoint;

	// Grab ledge check
	if (UKismetMathLibrary::NotEqual_VectorVector(_vWallHeightLocation, FVector(0.0f, 0.0f, 0.0f), 10.0f))
	{
		// Distance check
		_bIsTryingToVault = GetHipToLedge();
		if (_bIsTryingToVault)
		{
			// Cancel any actions
			_bPrimaryReloadCancelled = true;
			_bSecondaryReloadCancelled = true;

			// Play animation FP
			uint8 byteHandAnim = (uint8)E_HandAnimation::eHA_Equip;
			uint8 byteGunAnim = (uint8)0;
			OwningClient_PlayPrimaryWeaponFPAnimation(1.0f, false, true, byteHandAnim, 0.f, false, byteGunAnim, 0.0f);

			// Grab ledge (on ALL clients)
			FVector moveLocation = GetMoveToLocation(_fVaultHeightOffset, _fVaultForwardOffset);
			if (Role == ROLE_Authority) { Multicast_Reliable_GrabLedge(moveLocation); }
			else { Server_Reliable_GrabLedge(moveLocation); }

			// Set IsCliming/Vaulting
			if (Role == ROLE_Authority)
			{ _bIsVaulting = true; } 
			else
			{ Server_Reliable_SetIsVaulting(true); }
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
FHitResult ABaseCharacter::LedgeForwardTrace()
{
	// Get trace vectors
	FVector traceStart = GetActorLocation();
	FVector traceEnd = traceStart;
	FVector forwardVec = UKismetMathLibrary::GetForwardVector(GetActorRotation());
	FVector additive = FVector(forwardVec.X * _fLedgeForwardTraceLength, forwardVec.Y * _fLedgeForwardTraceLength, forwardVec.Z);
	traceEnd += additive;

	// Fire sphere trace
	FHitResult hitResult;
	bool traceComplex = false;
	TArray<AActor*> ignoreList;
	ignoreList.Add(this);
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), traceStart, traceEnd, _fLedgeForwardTraceRadius, _eVaultTraceChannel, traceComplex, ignoreList, EDrawDebugTrace::None, hitResult, true);

	return hitResult;
}

///////////////////////////////////////////////

/*
*
*/
FHitResult ABaseCharacter::LedgeHeightTrace()
{
	// Get trace vectors
	FVector traceStart = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + _fLedgeHeightTraceVerticalOffset);
	FVector traceEnd = traceStart;
	FVector forwardVec = UKismetMathLibrary::GetForwardVector(GetActorRotation());
	FVector additive = forwardVec * _fLedgeHeightTraceForwardOffset;
	traceStart += additive;
	traceEnd = FVector(traceStart.X, traceStart.Y, traceStart.Z - _fLedgeHeightTraceLength);

	// Fire sphere trace
	FHitResult hitResult;
	bool traceComplex = false;
	TArray<AActor*> ignoreList;
	ignoreList.Add(this);
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), traceStart, traceEnd, _fLedgeForwardTraceRadius, _eVaultTraceChannel, traceComplex, ignoreList, EDrawDebugTrace::None, hitResult, true);

	return hitResult;
}

///////////////////////////////////////////////

/*
*
*/
bool ABaseCharacter::GetHipToLedge()
{
	// Sanity check
	if (GetMesh() == NULL) { return false; }

	// Determine range
	FVector socketLocation = GetMesh()->GetSocketLocation(_sPelvisSocket);
	float range = socketLocation.Z - _vWallHeightLocation.Z;

	return UKismetMathLibrary::InRange_FloatFloat(range, _fLedgeGrabThresholdMin, _fLedgeGrabThresholdMax);
}

///////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::GrabLedge(FVector MoveLocation)
{
	// Disable movement mode
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCharacterMovement()->DisableMovement();

	FLatentActionInfo info = FLatentActionInfo();
	info.CallbackTarget = this;
	info.ExecutionFunction = FName("ClimbLedge");
	info.Linkage = 0;
	info.UUID = GetNextUUID();
	UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), MoveLocation, GetActorRotation(), false, false, _fVaultTime, false, EMoveComponentAction::Move, info);
}

bool ABaseCharacter::Server_Reliable_GrabLedge_Validate(FVector MoveLocation)
{ return true; }

void ABaseCharacter::Server_Reliable_GrabLedge_Implementation(FVector MoveLocation)
{
	Multicast_Reliable_GrabLedge(MoveLocation);
}

bool ABaseCharacter::Multicast_Reliable_GrabLedge_Validate(FVector MoveLocation)
{ return true; }

void ABaseCharacter::Multicast_Reliable_GrabLedge_Implementation(FVector MoveLocation)
{
	GrabLedge(MoveLocation);
}

///////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::ClimbLedge()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	// No longer climbing/vaulting
	if (Role == ROLE_Authority)
	{ _bIsVaulting = false; }
	else
	{ Server_Reliable_SetIsVaulting(false); }

	_bIsTryingToVault = false;
}

///////////////////////////////////////////////

bool ABaseCharacter::Server_Reliable_SetIsVaulting_Validate(bool Vaulting)
{ return true; }

void ABaseCharacter::Server_Reliable_SetIsVaulting_Implementation(bool Vaulting)
{
	_bIsVaulting = Vaulting;
}

///////////////////////////////////////////////

/*
*
*/
FVector ABaseCharacter::GetMoveToLocation(float HeightOffset, float ForwardOffset)
{
	FVector wallNormal = _vWallNormal * FVector(34.0f, 34.0f, 0.0f);
	FVector height = FVector(wallNormal.X + _vWallImpactPoint.X, wallNormal.Y + _vWallImpactPoint.Y, _vWallHeightLocation.Z + HeightOffset);
	FVector forward = _vWallTraceEnd - _vWallTraceStart;
	forward.Normalize(1.0f);
	forward *= ForwardOffset;
	///DrawDebugLine(GetWorld(), _vWallTraceStart, _vWallTraceStart + forward, FColor::Blue, true, 10.0f);

	return height + forward;
}
