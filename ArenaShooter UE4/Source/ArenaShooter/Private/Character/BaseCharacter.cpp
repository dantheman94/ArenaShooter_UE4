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
#include "WeaponAnimInstance.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Startup

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	SetReplicateMovement(true);
	///SetRemoteRoleForBackwardsCompat(ROLE_AutonomousProxy);

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

	// First person arms duel left
	_FirstPerson_ArmsDuelLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonArmsDuelLeft"));
	_FirstPerson_ArmsDuelLeft->SetupAttachment(_FirstPerson_SpringArm);
	_FirstPerson_ArmsDuelLeft->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -167.5f), FRotator(0.0f, 0.0f, -90.0f));
	_FirstPerson_ArmsDuelLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_FirstPerson_ArmsDuelLeft->SetOnlyOwnerSee(true);
	_FirstPerson_ArmsDuelLeft->SetVisibility(false);

	// First person arms duel right
	_FirstPerson_ArmsDuelRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonArmsDuelRight"));
	_FirstPerson_ArmsDuelRight->SetupAttachment(_FirstPerson_SpringArm);
	_FirstPerson_ArmsDuelRight->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -167.5f), FRotator(0.0f, 0.0f, -90.0f));
	_FirstPerson_ArmsDuelRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_FirstPerson_ArmsDuelRight->SetOnlyOwnerSee(true);
	_FirstPerson_ArmsDuelRight->SetVisibility(false);

	// First person primary weapon mesh
	_FirstPerson_PrimaryWeapon_SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonPrimaryWeaponMesh"));
	_FirstPerson_PrimaryWeapon_SkeletalMesh->SetupAttachment(_FirstPerson_Arms, "hand_r");
	_FirstPerson_PrimaryWeapon_SkeletalMesh->SetOnlyOwnerSee(true);
	_FirstPerson_PrimaryWeapon_SkeletalMesh->bSelfShadowOnly = true;
	_FirstPerson_PrimaryWeapon_SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// First person primary attachments
	FName sightAttachmentPoint = _PrimaryWeapon != NULL ? _PrimaryWeapon->GetSightAttachmentName() : "SightAttachmentPoint";
	_FirstPerson_PrimaryWeapon_Sight_StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirstPersonPrimaryWeaponSightMesh"));
	_FirstPerson_PrimaryWeapon_Sight_StaticMesh->SetupAttachment(_FirstPerson_PrimaryWeapon_SkeletalMesh, sightAttachmentPoint);
	_FirstPerson_PrimaryWeapon_Sight_StaticMesh->SetOnlyOwnerSee(true);
	_FirstPerson_PrimaryWeapon_Sight_StaticMesh->bSelfShadowOnly = true;
	_FirstPerson_PrimaryWeapon_Sight_StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// First person secondary weapon mesh
	_FirstPerson_SecondaryWeapon_SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPerson_DuelSecondaryWeaponMesh"));
	_FirstPerson_SecondaryWeapon_SkeletalMesh->SetupAttachment(_FirstPerson_ArmsDuelLeft, "hand_l");
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
	_fDefaultCapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	///GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Orange, TEXT("" + FString::SanitizeFloat(_fDefaultCapsuleHalfHeight)));
	///GEngine->AddOnScreenDebugMessage(11, 5.0f, FColor::Green, TEXT("" + FString::SanitizeFloat(GetCharacterMovement()->CrouchedHalfHeight)));

	// Get default movement values
	_fDefaultAirControl = GetCharacterMovement()->AirControl;
	_fDefaultGravityScale = GetCharacterMovement()->GravityScale;
	_fDefaultGroundFriction = GetCharacterMovement()->GroundFriction;
	_fDefaultBrakingFrictionFactor = GetCharacterMovement()->BrakingFrictionFactor;
	_fDefaultBrakingDecelerationWalking = GetCharacterMovement()->BrakingDecelerationWalking;
}

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
	DOREPLIFETIME(ABaseCharacter, _OldPrimaryWeapon);
	DOREPLIFETIME(ABaseCharacter, _OldReserveWeapon);
	DOREPLIFETIME(ABaseCharacter, _ReserveWeapon);
	DOREPLIFETIME(ABaseCharacter, _SecondaryWeapon);
}

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Animation

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::FreezeAnimation(USkeletalMeshComponent* SkeletalMesh, UAnimMontage* MontageToFreeze, float EndFrame, bool bHideMeshOnFreeze)
{
	UAnimInstance* animInstance = SkeletalMesh->GetAnimInstance();
	if (animInstance == NULL) { return; }

	///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, TEXT("Endframe: " + FString::SanitizeFloat(EndFrame)));

	if (!_bCancelAnimation)
	{ 
		animInstance->Montage_Play(MontageToFreeze, 0.0f, EMontagePlayReturnType::MontageLength, EndFrame, true);
		animInstance->Montage_Pause(MontageToFreeze);
		if (SkeletalMesh != NULL)
		{
			SkeletalMesh->bPauseAnims = true;
			///SkeletalMesh->SetHiddenInGame(bHideMeshOnFreeze, true);
		}
	}
	else { _bCancelAnimation = false; }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Current Frame

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	// If we're currently in the air and not doing any checks for landing on the ground, then start doing those checks
	if (GetCharacterMovement()->IsFalling() && !_bIsPerformingGroundChecks) { _bIsPerformingGroundChecks; }
	if (_bIsPerformingGroundChecks) { OnGroundChecks(); }

	// Recharging shields
	if (_bRechargeShields && _fShield < _MAX_SHIELD && GetLocalRole() == ROLE_Authority)
	{
		_fShield += _fShieldRechargingRate * DeltaTime;

		// Clamp to max
		if (_fShield >= _MAX_SHIELD)
		{
			_fShield = _MAX_SHIELD;
			_bRechargeShields = false;
		}
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
	else // Stopped trying to fire primary
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
	if (_bIsTryingToFireSecondary) { InitFireSecondaryWeapon(); } 
	else // Stopped trying to fire Secondary
	{
		if (_SecondaryWeapon == NULL) { return; }
		if (_SecondaryWeapon->GetFireModes().Num() == 0) { return; }
		if (_SecondaryWeapon->GetCurrentFireMode()->IsFiring())
		{
			// Only setting to FALSE if _bIsFiring == true
			_SecondaryWeapon->GetCurrentFireMode()->SetIsFiring(false);
		}
	}

	// Reloading weapons
	if (_bIsReloadingPrimaryWeapon)		{ UpdateReloadingPrimary(); }
	if (_bIsReloadingSecondaryWeapon)	{ UpdateReloadingSecondary(); }

	// Crouch camera lerping
	if (_bLerpCrouchCamera)
	{
		if (_PrimaryWeapon == NULL) { return; }

		// Add to lerp time
		if (_fCrouchCameraLerpTime < _fCrouchLerpingDuration)
		{
			_fCrouchCameraLerpTime += DeltaTime;
			
			// Determine origin transform
			bool ads = _PrimaryWeapon->GetCurrentFireMode()->IsAimDownSightEnabled();
			FTransform adsOrigin = _PrimaryWeapon->GetCurrentFireMode()->GetOriginADS();
			FTransform hipOrigin = _PrimaryWeapon->GetTransformOriginArms();
			FTransform originTransform = UKismetMathLibrary::SelectTransform(adsOrigin, hipOrigin, _bIsAiming && ads);

			// Get crouch/uncrouch origins
			FTransform crouchTransform = UKismetMathLibrary::SelectTransform(_tCrouchWeaponOrigin, originTransform, _bIsCrouching);
			FTransform uncrouchTransform = UKismetMathLibrary::SelectTransform(originTransform, _tCrouchWeaponOrigin, _bIsCrouching);

			// Current lerp transition origin between crouch/uncrouch
			FTransform enter = UKismetMathLibrary::SelectTransform(crouchTransform, uncrouchTransform, _bCrouchEnter);
			FTransform exit = UKismetMathLibrary::SelectTransform(crouchTransform, uncrouchTransform, !_bCrouchEnter);
			FTransform lerpTransform = UKismetMathLibrary::TLerp(uncrouchTransform, crouchTransform, _fCrouchCameraLerpTime / _fCrouchLerpingDuration);

			// Set lerp transform to first person arms
			_FirstPerson_Arms->SetRelativeTransform(lerpTransform);
		}

		// Stop lerping
		else
		{ _bLerpCrouchCamera = false; }
	}

	// Crouch movement capsule lerping
	if (_bLerpCrouchCapsule)
	{
		// Add to lerp time
		if (_fCrouchCapsuleLerpTime < _fCrouchLerpingDuration)
		{
			_fCrouchCapsuleLerpTime += DeltaTime;

			// Get capsule height values
			float lerpA = UKismetMathLibrary::SelectFloat(_fDefaultCapsuleHalfHeight, GetCharacterMovement()->CrouchedHalfHeight, _bIsCrouching);
			float lerpB = UKismetMathLibrary::SelectFloat(_fDefaultCapsuleHalfHeight, GetCharacterMovement()->CrouchedHalfHeight, !_bIsCrouching);

			// Lerp between values over time
			float currentLerpHeight = UKismetMathLibrary::Lerp(lerpA, lerpB, _fCrouchCapsuleLerpTime / _fCrouchLerpingDuration);
			UCapsuleComponent* movementCapsule = GetCapsuleComponent();
			if (movementCapsule == NULL) { return; }
			movementCapsule->SetCapsuleHalfHeight(currentLerpHeight, true);
		}
	}

	// Sprint
	if (_bTryingToSprint) { Tick_Sprint(); }
	else
	{
		if (_bIsSprinting) { StopSprinting(); }
	}

}

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

		// Set gravity scale to default
		GetCharacterMovement()->GravityScale = _fDefaultGravityScale;

		_bIsPerformingGroundChecks = false;
	}

	// Character is falling
	else { _fFallingVelocity = GetVelocity().Z; }
}

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
		USkeletalMeshComponent* mesh = !_bIsDuelWielding ? _FirstPerson_Arms : _FirstPerson_ArmsDuelRight;
		UAnimInstance* animInstance = mesh->GetAnimInstance();
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

/*
*
*/
void ABaseCharacter::UpdateReloadingSecondary()
{
	// Sanity checks
	if (_SecondaryWeapon == NULL) { return; }
	if (_SecondaryWeapon->GetFireModes().Num() == 0) { return; }
	if (_SecondaryWeapon->GetCurrentFireMode() == NULL) { return; }

	// Reload hasn't been canceled yet
	if (!_bSecondaryReloadCancelled)
	{
		// Ensure that _bIsReloadingSecondaryWeapon == true
		if (_bIsReloadingSecondaryWeapon == false) { Server_Reliable_SetIsReloadingSecondaryWeapon(true); }

		// Ensure that we cannot fire this weapon whilst it is still being reloaded
		_bCanFireSecondary = false;
	}

	// Reload canceled
	else
	{
		// Stop the reload timer
		FTimerHandle timerHandle = _SecondaryWeapon->GetCurrentFireMode()->GetReloadTimerHandle();
		GetWorldTimerManager().ClearTimer(timerHandle);

		// Can (try) to shoot weapon again
		_bCanFireSecondary = true;
		Server_Reliable_SetIsReloadingSecondaryWeapon(false);
		_bSecondaryReloadCancelled = false;

		// Stop montage
		UAnimInstance* animInstance = _FirstPerson_ArmsDuelLeft->GetAnimInstance();
		if (animInstance == NULL) { return; }
		animInstance->StopAllMontages(0.0f);
	}

	// On reload complete
	if (_SecondaryWeapon->GetCurrentFireMode()->GetReloadComplete())
	{
		_bCanFireSecondary = true;
		Server_Reliable_SetIsReloadingSecondaryWeapon(false);
		_bSecondaryReloadCancelled = false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Camera

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ABaseCharacter::OwningClient_PlayCameraShake_Validate(TSubclassOf<class UCameraShake>, float Scale = 1.0f)
{ return GetController() != NULL; }

void ABaseCharacter::OwningClient_PlayCameraShake_Implementation(TSubclassOf<class UCameraShake> ShakeClass, float Scale = 1.0f)
{
	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (playerController != NULL && ShakeClass != NULL)
	{
		playerController->PlayerCameraManager->PlayCameraShake(ShakeClass, Scale, ECameraAnimPlaySpace::CameraLocal, FRotator::ZeroRotator);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Controller | Gamepad

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ABaseCharacter::OwningClient_GamepadRumble_Validate(float Intensity, float Duration, 
	bool AffectsLeftLarge, bool AffectsLeftSmall, bool AffectsRightLarge, bool AffectsRightSmall)
{ return GetController() != NULL; }

void ABaseCharacter::OwningClient_GamepadRumble_Implementation(float Intensity, float Duration, 
	bool AffectsLeftLarge, bool AffectsLeftSmall, bool AffectsRightLarge, bool AffectsRightSmall)
{
	auto ctrl = Cast<ABasePlayerController>(GetController());
	ctrl->GamepadRumble(Intensity, Duration, AffectsLeftLarge, AffectsLeftSmall, AffectsRightLarge, AffectsRightSmall);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Controller | Input

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void ABaseCharacter::SetForwardInputScale(float ForwardInputScale)
{
	if (GetLocalRole() == ROLE_Authority)
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

void ABaseCharacter::SetRightInputScale(float RightInputScale)
{
	if (GetLocalRole() == ROLE_Authority)
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Health |

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ABaseCharacter::OnAnyDamage(AActor* Actor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		// Is taking damage
		_bIsTakingDamage = true;
		_fOnTakeDamage.Broadcast(Damage);
		ResetShieldRecharge();

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

void ABaseCharacter::OnPointDamage(float Damage)
{

}

void ABaseCharacter::OnRadialDamage(float Damage)
{

}

void ABaseCharacter::OnDeath()
{
	if (GetLocalRole() == ROLE_Authority)
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Health | Burn

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Health | Flesh

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Health | Shield

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

/*
*
*/
void ABaseCharacter::ResetShieldRecharge()
{
	// Reset timer
	GetWorld()->GetTimerManager().ClearTimer(_fShieldRechargeDelayHandle);
	_bRechargeShields = false;

	// Start timer
	FTimerDelegate rechargeDelegate;
	rechargeDelegate.BindUFunction(this, FName("StartRechargingShields"));
	GetWorld()->GetTimerManager().SetTimer(_fShieldRechargeDelayHandle, rechargeDelegate, 1.0f, false, _fShieldRechargeDelay);
}

/*
*
*/
void ABaseCharacter::StartRechargingShields()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		_bIsTakingDamage = false;
		_bRechargeShields = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Interaction

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

/*
*
*/
void ABaseCharacter::AddToInteractablesArray(AInteractable* Interactable)
{
	_Interactables.Add(Interactable);
}

/*
*
*/
void ABaseCharacter::RemoveFromInteractablesArray(AInteractable* Interactable)
{
	if (_Interactables.Contains(Interactable)) { _Interactables.Remove(Interactable); }
}

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

/*
*
*/
void ABaseCharacter::InteractSecondary()
{
	// Sanity check
	if (_FocusInteractable == NULL) { return; }

	// Start interaction timer
	FTimerDelegate interactionDelegate;
	interactionDelegate.BindUFunction(this, FName("Interact"), true);
	GetWorld()->GetTimerManager().SetTimer(_fInteractionHandle, interactionDelegate, 1.0f, false, _fInteractionThresholdTime);
}

/*
*
*/
void ABaseCharacter::CancelInteraction()
{
	// Cancel interaction timer
	GetWorld()->GetTimerManager().ClearTimer(_fInteractionHandle);
}

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
		Server_Reliable_SpawnAbstractWeapon(IsSecondary, weaponClass, magSize, reserveSize, batterySize);

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Inventory | Starting

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		if (_StartingPrimaryWeaponClass != NULL)
		{
			// Spawn weapon & set
			AWeapon* weapon = GetWorld()->SpawnActor<AWeapon>(_StartingPrimaryWeaponClass, location, rotation, spawnInfo);
			Server_Reliable_SetPrimaryWeapon(weapon, true, true);
		}

		// Secondary (left hand)
		if (_StartingSecondaryWeaponClass != NULL)
		{
			_bIsDuelWielding = true;

			// Spawn weapon & set
			AWeapon* weapon = GetWorld()->SpawnActor<AWeapon>(_StartingSecondaryWeaponClass, location, rotation, spawnInfo);
			Server_Reliable_SetSecondaryWeapon(weapon);
		}
		else { _bIsDuelWielding = false; }

		// Reserve (backpack) weapon
		if (_StartingReserveWeaponClass != NULL)
		{
			// Spawn weapon & set
			AWeapon* weapon = GetWorld()->SpawnActor<AWeapon>(_StartingReserveWeaponClass, location, rotation, spawnInfo);
			Server_Reliable_SetReserveWeapon(weapon);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Inventory | Weapon | Aiming

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

/*
*
*/
void ABaseCharacter::AimWeaponEnter()
{
	// Character sanity checks
	if (_bIsReloadingPrimaryWeapon || _bIsReloadingSecondaryWeapon || _bIsTogglingWeapons) { return; }
	if (!_bCanAim) { return; }
	if (_bIsSprinting) { StopSprinting(); }
	if (_bIsDuelWielding) { return; }

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
	if (HasAuthority()) { _bIsAiming = true; } else { Server_Reliable_SetIsAiming(true); }

	// Init lerp targets
	_fFovStart = _FirstPerson_Camera->FieldOfView;
	_fFovTarget = _PrimaryWeapon->GetCurrentFireMode()->GetFovArray()[0];

	_eAimDirection = E_AimDirection::ePT_ZoomIn;
	_bIsAimLerping = true;

	// Transform origin lerping
	if (_PrimaryWeapon->GetCurrentFireMode()->IsAimDownSightEnabled()) { _fAdsAnimationEvent.Broadcast(true); }
}

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Inventory | Weapon | Firing

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::FireWeaponTraceFullAuto(AWeapon* Weapon)
{
	// Fire delay complete?
	if (Weapon->GetCurrentFireMode()->FireDelayComplete())
	{
		// Fire
		if (Weapon->IsOwnersPrimaryWeapon())
		{ OwningClient_Reliable_PrimaryWeaponCameraTrace(); } 
		else if (Weapon->IsOwnersSecondaryWeapon())
		{ OwningClient_Reliable_SecondaryWeaponCameraTrace(); }
		else { return; }
	}
}

/*
*
*/
void ABaseCharacter::FireWeaponTraceSemiAuto(AWeapon* Weapon)
{
	// Fire delay complete?
	if (Weapon->GetCurrentFireMode()->FireDelayComplete())
	{
		if (_bHasReleasedPrimaryTrigger)
		{
			// Fire
			if (Weapon->IsOwnersPrimaryWeapon())
			{ OwningClient_Reliable_PrimaryWeaponCameraTrace(); } 
			else if (Weapon->IsOwnersSecondaryWeapon())
			{ OwningClient_Reliable_SecondaryWeaponCameraTrace(); }
			else { return; }
		}
	}
}

/*
*
*/
void ABaseCharacter::FireWeaponTraceBurst(AWeapon* Weapon)
{
	// Fire delay complete? (Are we mid-burst or?)
	if (Weapon->GetCurrentFireMode()->IsMidBurst())
	{

	}
	else
	{

	}
	if (Weapon->GetCurrentFireMode()->FireDelayComplete())
	{
		// Fire
		if (Weapon->IsOwnersPrimaryWeapon())
		{ OwningClient_Reliable_PrimaryWeaponCameraTrace(); } 
		else if (Weapon->IsOwnersSecondaryWeapon())
		{ OwningClient_Reliable_SecondaryWeaponCameraTrace(); }
		else { return; }
	}
}

/*
*
*/
void ABaseCharacter::FireWeaponTraceSpread(AWeapon* Weapon)
{
	// Fire delay complete?
	if (Weapon->GetCurrentFireMode()->FireDelayComplete())
	{
		// Fire
		for (int i = 0; i < Weapon->GetCurrentFireMode()->GetProjectileCountPerSpread(); i++)
		{
			if (Weapon->IsOwnersPrimaryWeapon())
			{ OwningClient_Reliable_PrimaryWeaponCameraTrace(); }
			else if (Weapon->IsOwnersSecondaryWeapon())
			{ OwningClient_Reliable_SecondaryWeaponCameraTrace(); }
			else { return; }
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Inventory | Weapon  | Toggle

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
void ABaseCharacter::InputToggleWeapon()
{
	// Sanity checks
	if (_PrimaryWeapon == NULL) { return; }
	if (_bIsTogglingWeapons == true) { return; }

	// Duel-wielding?
	if (_bIsDuelWielding)
	{
		// Cancel any reloads that may be happening
		_bSecondaryReloadCancelled = true;

		// Drop secondary weapon
		Server_Reliable_DropInteractableWeapon(_SecondaryWeapon);
		Server_Reliable_SetSecondaryWeapon(NULL);
	}
	else
	{
		// Only proceed if there is a reserve weapon to toggle with
		if (_ReserveWeapon == NULL) { return; }

		///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Toggle"));

		// Cancel any reloads that may be happening
		_bPrimaryReloadCancelled = true;
		
		// Setup toggle environment (old primary/reserve pointer refs) && notify state change
		if (GetLocalRole() == ROLE_Authority) { SetupToggleWeapon(); }
		else { Server_Reliable_SetupToggleWeapon(); }

		// Play lower animation
		uint8 handAnim = (uint8)E_HandAnimation::eHA_Unequip;
		UAnimMontage* lowerMontage = _PrimaryWeapon->GetArmAnimation((E_HandAnimation)handAnim);
		if (lowerMontage == NULL)
		{ 		
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("ERROR: " + AActor::GetDebugName(_PrimaryWeapon) + " has no [UNEQUIP / LOWER] animation reference. Cannot continue TOGGLING PRIMARY/RESERVE state."));
			return; 
		}
		OwningClient_PlayPrimaryWeaponFPAnimation(_FirstPerson_Arms, _fGlobalTogglePlayRate, false, true, handAnim, 0.0f, false, 0, 0.0f);
		float lowerDelay = lowerMontage->GetPlayLength();
		float toggleTimeBetweenAnimations = 0.5f;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, TEXT("Toggle delay: " + FString::SanitizeFloat(lowerDelay + toggleTimeBetweenAnimations)));

		// Start delay timer for the actual toggle
		FTimerDelegate toggleDelegate;
		toggleDelegate.BindUFunction(this, FName("ToggleWeapon"));
		GetWorld()->GetTimerManager().SetTimer(_fToggleHandle, toggleDelegate, 1.0f, false, lowerDelay + toggleTimeBetweenAnimations);

		// Start delay timer for the weapon raise
		///FTimerDelegate raiseDelegate;
		///raiseDelegate.BindUFunction(this, FName("ExitToggleWeapon"));
		///GetWorld()->GetTimerManager().SetTimer(_fExitToggleHandle, raiseDelegate, 1.0f, false, lowerDelay + (toggleTimeBetweenAnimations * 4));
	}
}

/*
*
*/
bool ABaseCharacter::Server_Reliable_SetupToggleWeapon_Validate()
{ return true; }

void ABaseCharacter::Server_Reliable_SetupToggleWeapon_Implementation()
{
	SetupToggleWeapon();
}

/*
*	Setup toggle environment (old primary/reserve pointer refs) && notify state change
*/
void ABaseCharacter::SetupToggleWeapon()
{
	_bIsTogglingWeapons = true;

	// Set previous weapons
	_OldPrimaryWeapon = _PrimaryWeapon;
	_OldReserveWeapon = _ReserveWeapon;
}

/*
*
*/
bool ABaseCharacter::Server_Reliable_ToggleWeapon_Validate()
{ return true; }

void ABaseCharacter::Server_Reliable_ToggleWeapon_Implementation()
{
	ToggleWeapon();
}

/*
*
*/
void ABaseCharacter::ToggleWeapon()
{

	GetWorldTimerManager().ClearTimer(_fToggleHandle);

	// Only perform on the server
	if (GetLocalRole() == ROLE_Authority)
	{
		// Swap the weapons
		Server_Reliable_SetPrimaryWeapon(_OldReserveWeapon, false, false, false);
		Server_Reliable_SetReserveWeapon(_OldPrimaryWeapon);

		// Raise animation
		ExitToggleWeapon();
	}

	// Call this function again, but force it as a server RPC call
	else
	{ Server_Reliable_ToggleWeapon(); }
}

/*
*
*/
bool ABaseCharacter::Server_Reliable_ExitToggleWeapon_Validate()
{ return true; }

void ABaseCharacter::Server_Reliable_ExitToggleWeapon_Implementation()
{
	ExitToggleWeapon();
}

/*
*
*/
void ABaseCharacter::ExitToggleWeapon()
{
	GetWorldTimerManager().ClearTimer(_fExitToggleHandle);

	// Only perform on server
	if (GetLocalRole() == ROLE_Authority)
	{
		_bIsTogglingWeapons = false;

		// Play raise/equip animation (on client)
		uint8 handAnim = (uint8)E_HandAnimation::eHA_Equip;
		OwningClient_PlayPrimaryWeaponFPAnimation(_FirstPerson_Arms, _fGlobalTogglePlayRate, false, true, handAnim, 0.0f, false, 0, 0.0f);

		// Can only fire once the raise animation is complete
		UAnimMontage* raiseMontage = _PrimaryWeapon->GetArmAnimation((E_HandAnimation)handAnim);
		float raiseDelay = raiseMontage->GetPlayLength() + 0.5f;

	} 

	// Call this function again, but force it as a server RPC call
	else
	{ Server_Reliable_ExitToggleWeapon(); }
}

/*
*
*/
bool ABaseCharacter::Server_Reliable_DropInteractableWeapon_Validate(AWeapon* WeaponInstance)
{ return true; }

void ABaseCharacter::Server_Reliable_DropInteractableWeapon_Implementation(AWeapon* WeaponInstance)
{
	// Sanity check
	if (WeaponInstance == NULL) { return; }

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

/*
*	Spawns the abstract Weapon actor (not to be mistaken with 'Interactable_Weapon') & set's it to the characters loadout
*/
bool ABaseCharacter::Server_Reliable_SpawnAbstractWeapon_Validate(bool IsSecondary, TSubclassOf<AWeapon> WeaponClass, int MagazineSize, int ReserveSize, int BatterySize)
{ return true; }

void ABaseCharacter::Server_Reliable_SpawnAbstractWeapon_Implementation(bool IsSecondary, TSubclassOf<AWeapon> WeaponClass, int MagazineSize, int ReserveSize, int BatterySize)
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
		Server_Reliable_DropInteractableWeapon(_PrimaryWeapon);
		
		// If we're already dual-wielding, don't play a raise animation
		bool bPlayAnimation = _bIsDuelWielding ? false : true;
		Server_Reliable_SetPrimaryWeapon(weapon, bPlayAnimation, true);
	} else
	{
		Server_Reliable_DropInteractableWeapon(_SecondaryWeapon);
		Server_Reliable_SetSecondaryWeapon(weapon);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Inventory | Weapon | Primary

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* @summary:	Sets the character's primary weapon
*
* @networking:	Runs on server
*
* @param:	AWeapon* Weapon
*
* @return:	void
*/
bool ABaseCharacter::Server_Reliable_SetPrimaryWeapon_Validate(AWeapon* Weapon, bool PlayAnimation, bool FirstPickup, bool DestroyOld = true)
{ return true; }

void ABaseCharacter::Server_Reliable_SetPrimaryWeapon_Implementation(AWeapon* Weapon, bool PlayAnimation, bool FirstPickup, bool DestroyOld = true)
{
	// Destroy/despawn the old weapon once the new weapon has been set
	AWeapon* destroyActor = _PrimaryWeapon;
	
	// Set weapon owner
	_PrimaryWeapon = Weapon;
	if (_PrimaryWeapon != NULL)
	{
		_PrimaryWeapon->SetOwner(this);
		_PrimaryWeapon->Server_Reliable_SetNewOwner(this);
		_PrimaryWeapon->Server_Reliable_SetOwnersPrimaryWeapon(true);
		_PrimaryWeapon->Server_Reliable_SetOwnersSecondaryWeapon(false);
	}

	// destroy the old primary weapon actor (the abstract actor instance)
	if (destroyActor != NULL && DestroyOld)
		destroyActor->Destroy();

	// Update meshes (done in the OnRep() functions)
	if (GetLocalRole() == ROLE_Authority) { OnRep_PrimaryWeapon(); OnRep_SecondaryWeapon(); }
}

/*
*
*/
bool ABaseCharacter::OwningClient_UpdateFirstPersonPrimaryWeaponMesh_Validate(AWeapon* Weapon, bool PlayAnimation, bool FirstPickup)
{ return true; }

void ABaseCharacter::OwningClient_UpdateFirstPersonPrimaryWeaponMesh_Implementation(AWeapon* Weapon, bool PlayAnimation, bool FirstPickup)
{
	// Either update the mesh with the mesh referenced from the weapon, otherwise clear the mesh on the character
	if (Weapon != NULL)
	{
		_FirstPerson_PrimaryWeapon_SkeletalMesh->SetSkeletalMesh(Weapon->GetFirstPersonMesh());
		
		// Update origins
		if (_bIsDuelWielding)
		{
			// hide / show FPS arms
			_FirstPerson_Arms->SetHiddenInGame(true);
			_FirstPerson_ArmsDuelLeft->SetHiddenInGame(false);
			_FirstPerson_ArmsDuelRight->SetHiddenInGame(false);

			// Attach primary weapon mesh to fps right hand duel wielding mesh
			_FirstPerson_PrimaryWeapon_SkeletalMesh->AttachToComponent(_FirstPerson_ArmsDuelRight, FAttachmentTransformRules::KeepWorldTransform, "hand_r");

			// Set weapon's relative transform offset to the fps arms transform
			_FirstPerson_ArmsDuelRight->SetRelativeTransform(Weapon->GetTransformOriginDuelPrimaryArms());
			_FirstPerson_PrimaryWeapon_SkeletalMesh->SetRelativeTransform(Weapon->GetTransformOriginDuelPrimaryWeapon());

			// Reset origin starting location in arena character (used for first person dash animation)
			///AArenaCharacter* arenaChar = Cast<AArenaCharacter>(this);
			///if (arenaChar) { arenaChar->SetFPRelativeStartingTransform(Weapon->GetTransformOriginArms()); }

			// Set animation BP instance (anim BP)
			_FirstPerson_ArmsDuelRight->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			_FirstPerson_ArmsDuelRight->SetAnimInstanceClass(Weapon->GetAnimInstanceFirstPersonHands());
			UWeaponAnimInstance* wepAnimInst = Cast<UWeaponAnimInstance>(_FirstPerson_ArmsDuelRight->GetAnimInstance());
			if (wepAnimInst != NULL) { wepAnimInst->SetWeapon(_PrimaryWeapon); }

			// Set animation BP instance (anim BP) for the weapon mesh
			_FirstPerson_PrimaryWeapon_SkeletalMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			_FirstPerson_PrimaryWeapon_SkeletalMesh->SetAnimInstanceClass(Weapon->GetAnimInstanceFirstPersonGun());

			// Play raise animation
			uint8 handAnim = (uint8)E_HandAnimation::eHA_EquipDuelRight;
			uint8 gunAnim = (uint8)0;
			OwningClient_PlayPrimaryWeaponFPAnimation(_FirstPerson_ArmsDuelRight, 1.0f, false, true, handAnim, 0.0f, false, gunAnim, 0.0f);
		} 

		// _bIsDuelWielding == FALSE
		else
		{
			// hide / show FPS arms
			_FirstPerson_Arms->SetHiddenInGame(false);
			_FirstPerson_ArmsDuelLeft->SetHiddenInGame(true);
			_FirstPerson_ArmsDuelRight->SetHiddenInGame(true);

			// Attach primary weapon to default fps arms mesh
			_FirstPerson_PrimaryWeapon_SkeletalMesh->AttachToComponent(_FirstPerson_Arms, FAttachmentTransformRules::KeepWorldTransform, "hand_r");

			// Set fps arms & weapon origin transforms
			_FirstPerson_Arms->SetRelativeTransform(Weapon->GetTransformOriginArms());
			_FirstPerson_PrimaryWeapon_SkeletalMesh->SetRelativeTransform(Weapon->GetTransformOriginWeapon());

			// Reset origin starting location in arena character (used for first person dash animation)
			AArenaCharacter* arenaChar = Cast<AArenaCharacter>(this);
			if (arenaChar) { arenaChar->SetFPRelativeStartingTransform(Weapon->GetTransformOriginArms()); }

			// Set animation BP instance (anim BP)
			_FirstPerson_Arms->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			_FirstPerson_Arms->SetAnimInstanceClass(Weapon->GetAnimInstanceFirstPersonHands());
			UWeaponAnimInstance* wepAnimInst = Cast<UWeaponAnimInstance>(_FirstPerson_Arms->GetAnimInstance());
			if (wepAnimInst != NULL) { wepAnimInst->SetWeapon(_PrimaryWeapon); }

			// Set animation BP instance (anim BP) for the weapon mesh
			_FirstPerson_PrimaryWeapon_SkeletalMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			_FirstPerson_PrimaryWeapon_SkeletalMesh->SetAnimInstanceClass(Weapon->GetAnimInstanceFirstPersonGun());

			// Play animations?
			if (PlayAnimation)
			{	
				// Play first pickup animation
				if (FirstPickup)
				{
					uint8 handAnim = (uint8)E_HandAnimation::eHA_FirstPickup;
					uint8 gunAnim = (uint8)E_GunAnimation::eGA_FirstPickup;
					OwningClient_PlayPrimaryWeaponFPAnimation(_FirstPerson_Arms, 1.0f, false, true, handAnim, 0.0f, true, gunAnim, 0.0f);
				}

				// OR Play standard raise animation
				else
				{
					uint8 handAnim = (uint8)E_HandAnimation::eHA_Equip;
					uint8 gunAnim = (uint8)0;
					OwningClient_PlayPrimaryWeaponFPAnimation(_FirstPerson_Arms, 1.0f, false, true, handAnim, 0.0f, false, gunAnim, 0.0f);
				}
			}
		}
	} 

	// Weapon == NULL
	else
	{
		// Nullify the animation BPs & skeletal meshes
		_FirstPerson_PrimaryWeapon_SkeletalMesh->SetAnimInstanceClass(NULL);
		_FirstPerson_PrimaryWeapon_SkeletalMesh->SetSkeletalMesh(NULL);
	}

	// Updating attachments for the weapon
	UpdateFirstPersonPrimaryScopeAttachment(Weapon);
}

/*
*
*/
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

/*
*
*/
void ABaseCharacter::UpdateFirstPersonPrimaryScopeAttachment(AWeapon* Weapon)
{
	// Either update the mesh with the mesh referenced from the weapon, otherwise clear the mesh on the character
	if (Weapon != NULL && Weapon->IsScopeAttachmentEnabled())
	{
		_FirstPerson_PrimaryWeapon_Sight_StaticMesh->SetStaticMesh(Weapon->GetSightMesh());

		// Ensure the scope is at the right attachment point
		FName sightAttachmentPoint = _PrimaryWeapon != NULL ? _PrimaryWeapon->GetSightAttachmentName() : "SightAttachmentPoint";
	}
	else
	{
		// Weapon == NULL
		_FirstPerson_PrimaryWeapon_Sight_StaticMesh->SetStaticMesh(NULL);
	}
}

/*
*
*/
void ABaseCharacter::OnRep_PrimaryWeapon()
{
	// This function is called when updating _SecondaryWeapon's value,
	// so if we're currently duel wielding -> DONT play a raise animation
	bool bPlayAnimation = _SecondaryWeapon != NULL ? true : false;
	
	// Update meshes
	OwningClient_UpdateFirstPersonPrimaryWeaponMesh(_PrimaryWeapon, bPlayAnimation, false);
	Multicast_UpdateThirdPersonPrimaryWeaponMesh(_PrimaryWeapon);
}

/*
*
*/
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
		switch (_PrimaryWeapon->GetCurrentFireMode()->GetFiringType())
		{
		case E_FiringModeType::eFMT_FullAuto: FireWeaponTraceFullAuto(_PrimaryWeapon); break;
		case E_FiringModeType::eFMT_Burst: FireWeaponTraceBurst(_PrimaryWeapon); break;
		case E_FiringModeType::eFMT_SemiAuto: FireWeaponTraceSemiAuto(_PrimaryWeapon); break;
		case E_FiringModeType::eFMT_Spread: FireWeaponTraceSpread(_PrimaryWeapon); break;
		default: break;
		}
	}
}

/*
*
*/
void ABaseCharacter::InputPrimaryFirePress()
{
	// Sanity check
	if (_PrimaryWeapon == NULL) { return; }
	if (_PrimaryWeapon->IsThereValidFireModeCount() == false) { return; }
	if (_PrimaryWeapon->GetCurrentFireMode() == NULL) { return; }

	// Only true if the trigger type is press enter
	if (_PrimaryWeapon->GetCurrentFireMode()->GetFiringTriggerType() == E_FiringTriggerType::eFTT_Press)
	{
		_bIsTryingToFirePrimary = true;
		_bHasReleasedPrimaryTrigger = false;
	} else
	{
		_bIsTryingToFirePrimary = false;
		_bHasReleasedPrimaryTrigger = true;
	}
}

/*
*
*/
void ABaseCharacter::InputPrimaryFireRelease()
{
	// Sanity check
	if (_PrimaryWeapon == NULL) { return; }
	if (_PrimaryWeapon->IsThereValidFireModeCount() == false) { return; }
	if (_PrimaryWeapon->GetCurrentFireMode() == NULL) { return; }

	// Only true if the trigger type is press exit
	if (_PrimaryWeapon->GetCurrentFireMode()->GetFiringTriggerType() == E_FiringTriggerType::eFTT_Release)
	{
		_bIsTryingToFirePrimary = true;
		_bHasReleasedPrimaryTrigger = false;
	} else
	{
		_bIsTryingToFirePrimary = false;
		_bHasReleasedPrimaryTrigger = true;
	}
}

/*
*
*/
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

	if (_bDebugDrawPrimaryWeaponCameraTrace)
	{
		DrawDebugLine(GetWorld(), worldLocation, traceEnd, _fPrimaryWeaponCameraTraceColour, false, 1.0f);
	}

	Server_Reliable_PrimaryWeaponCameraTrace(hitResult);
}

/*
*
*/
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
		UKismetMathLibrary::Vector_Normalize(v, 0.0001);
		FRotator rot = UKismetMathLibrary::MakeRotFromX(v);
		FQuat quat = FQuat(rot);
		trans.SetLocation(charMuzzle);
		trans.SetRotation(quat);

		FVector cameraForwardXVector = trans.GetRotation().Vector();

		// Start primary weapon fire
		_PrimaryWeapon->GetCurrentFireMode()->Fire(ClientHitResult, cameraForwardXVector, _FirstPerson_PrimaryWeapon_SkeletalMesh, _ThirdPerson_PrimaryWeapon_SkeletalMesh);
	}

	// !ClientHitResult.bBlockingHit
	else
	{
		// Get projectile transform
		FTransform trans = FTransform::Identity;
		FVector charMuzzle = _FirstPerson_PrimaryWeapon_SkeletalMesh->GetSocketLocation("MuzzleLaunchPoint");
		FVector v = FVector(ClientHitResult.TraceEnd - charMuzzle);
		UKismetMathLibrary::Vector_Normalize(v, 0.0001);
		FRotator rot = UKismetMathLibrary::MakeRotFromX(v);
		FQuat quat = FQuat(rot);
		trans.SetLocation(charMuzzle);
		trans.SetRotation(quat);

		FVector cameraForwardXVector = trans.GetRotation().Vector();

		// Start primary weapon fire
		_PrimaryWeapon->GetCurrentFireMode()->Fire(ClientHitResult, cameraForwardXVector, _FirstPerson_PrimaryWeapon_SkeletalMesh, _ThirdPerson_PrimaryWeapon_SkeletalMesh);
	}
}

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
						OwningClient_PlayPrimaryWeaponFPAnimation(_FirstPerson_Arms, _fGlobalReloadPlayRate, false, true, handAnimByte, startingFrame, true, gunAnimByte, startingFrame);

						return;
					}
				}
			}

			// Magazine full? Don't progress
			if (ammoPool->GetMagazineAmmo() >= ammoPool->GetMaximumMagazineAmmo()) { return; }

			// Cannot shoot whilst we are reloading
			_bCanFirePrimary = false;

			// Stop aiming
			if (_bIsAiming) { AimWeaponExit(); }

			// Start reloading state/sequence
			Server_Reliable_SetIsReloadingPrimaryWeapon(true);

			// Get reload montage reference (hands)
			uint8 handAnimByte;
			if (ammoPool->GetMagazineAmmo() >= 0 && ammoPool->IsRoundInChamber())
			{ handAnimByte = (uint8)E_HandAnimation::eHA_ReloadFullNotEmpty; } else
			{ handAnimByte = (uint8)E_HandAnimation::eHA_ReloadFullEmpty; }

			// Get reload montage reference (weapon)
			uint8 gunAnimByte;
			if (ammoPool->GetMagazineAmmo() >= 0 && ammoPool->IsRoundInChamber())
			{ gunAnimByte = (uint8)E_GunAnimation::eGA_ReloadFullNotEmpty; } else
			{ gunAnimByte = (uint8)E_GunAnimation::eGA_ReloadFullEmpty; }

			// Play reload animation
			float startingFrame = _PrimaryWeapon->GetCurrentFireMode()->GetReloadStartingTime();
			OwningClient_PlayPrimaryWeaponFPAnimation(_FirstPerson_Arms, _fGlobalReloadPlayRate, false, true, handAnimByte, startingFrame, true, gunAnimByte, startingFrame);

			break;
		}
		case E_ReloadType::eRT_LoadSingle: { break; }
		default: break;
		}
	}

	// Is duel wielding
	else
	{
		switch (fireMode->GetReloadType())
		{
		case E_ReloadType::eRT_Magazine:
		{
			// Doesn't have ammo in reserves? Don't progress
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
						// Dont play an animation because we're dual wielding.. instead just insta-chamber the round then continue on
						fireMode->Server_Reliable_ChamberRound();
					}
				}
			}

			// Magazine full? Don't progress
			if (ammoPool->GetMagazineAmmo() >= ammoPool->GetMaximumMagazineAmmo()) { return; }

			// Cannot shoot whilst we are reloading
			_bCanFirePrimary = false;

			// Stop aiming (we can't if we're dual wielding, but just incase..)
			if (_bIsAiming) { AimWeaponExit(); }

			// Start reloading state/sequence
			Server_Reliable_SetIsReloadingPrimaryWeapon(true);

			// Get reload lower montage references for playing
			uint8 handAnimByte; handAnimByte = (uint8)E_HandAnimation::eHA_ReloadDuelRightLower;
			uint8 gunAnimByte; gunAnimByte = (uint8)E_GunAnimation::eGA_ReloadDuelRightLower;

			// Play reload (lower) animation
			float startingFrame = _PrimaryWeapon->GetCurrentFireMode()->GetReloadStartingTime();
			OwningClient_PlayPrimaryWeaponFPAnimation(_FirstPerson_ArmsDuelRight, _fGlobalReloadPlayRate, true, true, handAnimByte, startingFrame, true, gunAnimByte, startingFrame);

			// Get timer duration (using a timer for the weapon raise animation)
			UAnimMontage* lowerMontage = _PrimaryWeapon->GetArmAnimation((E_HandAnimation)handAnimByte);
			float lowerAnimLength = lowerMontage->GetPlayLength();
			float reloadTime = _PrimaryWeapon->GetDuelWieldingReloadTime();

			// Start delay timer for the weapon raise
			FTimerDelegate raiseDelegate;
			raiseDelegate.BindUFunction(this, FName("OnDuelWielding_PrimaryReloadComplete"));
			GetWorld()->GetTimerManager().SetTimer(_fDuelPrimaryReloadHandle, raiseDelegate, 1.0f, false, lowerAnimLength + reloadTime);

			break;
		}
		case E_ReloadType::eRT_LoadSingle: { break; }
		default: break;
		}
	}
}

/*
*
*/
void ABaseCharacter::OnDuelWielding_PrimaryReloadComplete()
{
	uint8 handAnimByte; handAnimByte = (uint8)E_HandAnimation::eHA_ReloadDuelRightRaise;
	uint8 gunAnimByte; gunAnimByte = (uint8)E_GunAnimation::eGA_ReloadDuelRightRaise;

	// Play reload (raise) animation
	float startingFrame = 0.0f;
	OwningClient_PlayPrimaryWeaponFPAnimation(_FirstPerson_ArmsDuelRight, _fGlobalReloadPlayRate, false, true, handAnimByte, startingFrame, true, gunAnimByte, startingFrame);
}

/*
*
*/
bool ABaseCharacter::OwningClient_PlayPrimaryWeaponFPAnimation_Validate(USkeletalMeshComponent* ArmsMesh, float PlayRate, bool FreezeMontageAtLastFrame, bool PlayHandAnimation, uint8 HandAnimation, float HandAnimationStartingFrame, bool PlayGunAnimation, uint8 GunAnimation, float GunAnimationStartingFrame)
{ return true; }

void ABaseCharacter::OwningClient_PlayPrimaryWeaponFPAnimation_Implementation(USkeletalMeshComponent* ArmsMesh, float PlayRate, bool FreezeMontageAtLastFrame, bool PlayHandAnimation, uint8 HandAnimation, float HandAnimationStartingFrame, bool PlayGunAnimation, uint8 GunAnimation, float GunAnimationStartingFrame)
{
	PlayPrimaryWeaponFPAnimation(ArmsMesh, PlayRate, FreezeMontageAtLastFrame, PlayHandAnimation, HandAnimation, HandAnimationStartingFrame, PlayGunAnimation, GunAnimation, GunAnimationStartingFrame);
}

/*
*
*/
void ABaseCharacter::PlayPrimaryWeaponFPAnimation(USkeletalMeshComponent* ArmsMesh, float PlayRate, bool FreezeMontageAtLastFrame, bool PlayHandAnimation, uint8 HandAnimation, float HandAnimationStartingFrame, bool PlayGunAnimation, uint8 GunAnimation, float GunAnimationStartingFrame)
{	
	// Sanity checks
	if (_PrimaryWeapon == NULL) { return; }

	// Hand animation
	float i = 0.0f;
	if (PlayHandAnimation)
	{
		UAnimMontage* handMontage = _PrimaryWeapon->GetArmAnimation((E_HandAnimation)HandAnimation);
		UAnimInstance* handAnimInstance = ArmsMesh->GetAnimInstance();
		if (handMontage != NULL && handAnimInstance != NULL)
		{
			// Play hand montage
			if (ArmsMesh != NULL) { ArmsMesh->bPauseAnims = false; }
			ArmsMesh->SetHiddenInGame(false, true); // Force unhide the mesh before playing the animations
			handAnimInstance->Montage_Play(handMontage, PlayRate, EMontagePlayReturnType::MontageLength, HandAnimationStartingFrame, true);
			if (FreezeMontageAtLastFrame)
			{
				// Start delay timer then freeze anim
				float playLength = handMontage->GetPlayLength();
				FTimerDelegate animationDelegate;
				animationDelegate.BindUFunction(this, FName("FreezeAnimation"), ArmsMesh, handMontage, playLength);
				GetWorld()->GetTimerManager().SetTimer(_fPrimaryFPAnimationHandle, animationDelegate, 1.0f, false, (playLength / _fGlobalReloadPlayRate) - i);
			}
		}
	}

	// Gun animation
	if (PlayGunAnimation)
	{
		UAnimMontage* gunMontage = _PrimaryWeapon->GetGunAnimation((E_GunAnimation)GunAnimation);
		UAnimInstance* weaponAnimInstance = _FirstPerson_PrimaryWeapon_SkeletalMesh->GetAnimInstance();
		if (gunMontage != NULL && weaponAnimInstance != NULL)
		{
			// Play gun montage
			_FirstPerson_PrimaryWeapon_SkeletalMesh->SetHiddenInGame(false); // Force unhide the mesh before playing the animations
			weaponAnimInstance->Montage_Play(gunMontage, PlayRate, EMontagePlayReturnType::MontageLength, GunAnimationStartingFrame, true);
			if (FreezeMontageAtLastFrame)
			{
				// Start delay timer then freeze anim
				float playLength = gunMontage->GetPlayLength();
				FTimerDelegate animationDelegate;
				animationDelegate.BindUFunction(this, FName("FreezeAnimation"), _FirstPerson_PrimaryWeapon_SkeletalMesh, gunMontage, playLength);
				GetWorld()->GetTimerManager().SetTimer(_fPrimaryFPAnimationHandle, animationDelegate, 1.0f, false, (playLength / _fGlobalReloadPlayRate) - i);
			}
		}
	}
}

/*
*
*/
bool ABaseCharacter::Server_Reliable_SetIsReloadingPrimaryWeapon_Validate(bool ReloadingPrimary)
{ return true; }

void ABaseCharacter::Server_Reliable_SetIsReloadingPrimaryWeapon_Implementation(bool ReloadingPrimary)
{
	_bIsReloadingPrimaryWeapon = ReloadingPrimary;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Inventory | Weapon | Secondary

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
{ return true; }

void ABaseCharacter::Server_Reliable_SetSecondaryWeapon_Implementation(AWeapon* Weapon)
{	
	// Destroy/despawn the old weapon once the new weapon has been set
	AWeapon* destroyActor = _SecondaryWeapon;
	_SecondaryWeapon = Weapon;

	// Set weapon owner
	if (_SecondaryWeapon != NULL)
	{
		_SecondaryWeapon->SetOwner(this);
		_SecondaryWeapon->SetNewOwner(this);
		_SecondaryWeapon->SetOwnersPrimaryWeapon(false);
		_SecondaryWeapon->SetOwnersSecondaryWeapon(true);
	}

	// destroy the old secondary weapon actor (the abstract actor instance)
	if (destroyActor != NULL)
		destroyActor->Destroy();

	// Update meshes
	///OwningClient_UpdateFirstPersonPrimaryWeaponMesh(Weapon, FirstPickup);
	///Multicast_UpdateThirdPersonPrimaryWeaponMesh(Weapon);
	if (GetLocalRole() == ROLE_Authority) { OnRep_SecondaryWeapon(); OnRep_PrimaryWeapon(); }
}

/*
*
*/
bool ABaseCharacter::OwningClient_UpdateFirstPersonSecondaryWeaponMesh_Validate(AWeapon* Weapon)
{ return true; }

void ABaseCharacter::OwningClient_UpdateFirstPersonSecondaryWeaponMesh_Implementation(AWeapon* Weapon)
{
	if (_bStartingLoadout)
	{
		OwningClient_UpdateFirstPersonPrimaryWeaponMesh(_PrimaryWeapon, true, false);
		_bStartingLoadout = false;
	}
	
	// Either update the mesh with the mesh referenced from the weapon, otherwise clear the mesh on the character
	if (Weapon != NULL)
	{
		// Set mesh
		_FirstPerson_SecondaryWeapon_SkeletalMesh->SetSkeletalMesh(Weapon->GetFirstPersonMesh());

		// hide / show FPS arms
		_FirstPerson_Arms->SetHiddenInGame(true);
		_FirstPerson_ArmsDuelLeft->SetHiddenInGame(false);
		_FirstPerson_ArmsDuelRight->SetHiddenInGame(false);

		// Attach primary weapon mesh to fps right hand duel wielding mesh
		_FirstPerson_SecondaryWeapon_SkeletalMesh->AttachToComponent(_FirstPerson_ArmsDuelLeft, FAttachmentTransformRules::KeepWorldTransform, "hand_l");

		// Set weapon's relative transform offset to the fps arms transform
		_FirstPerson_ArmsDuelLeft->SetRelativeTransform(Weapon->GetTransformOriginDuelSecondaryArms());
		_FirstPerson_SecondaryWeapon_SkeletalMesh->SetRelativeTransform(Weapon->GetTransformOriginDuelSecondaryWeapon());

		// Reset origin starting location in arena character (used for first person dash animation)
		///AArenaCharacter* arenaChar = Cast<AArenaCharacter>(this);
		///if (arenaChar) { arenaChar->SetFPRelativeStartingTransform(Weapon->GetTransformOriginDuelSecondaryArms()); }

		// Set animation BP instance (anim BP)  for all fp arm meshes
		_FirstPerson_ArmsDuelLeft->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		_FirstPerson_ArmsDuelLeft->SetAnimInstanceClass(Weapon->GetAnimInstanceFirstPersonHands());
		UWeaponAnimInstance* wepAnimInst = Cast<UWeaponAnimInstance>(_FirstPerson_ArmsDuelLeft->GetAnimInstance());
		if (wepAnimInst != NULL) { wepAnimInst->SetWeapon(_SecondaryWeapon); }

		// Set animation BP instance (anim BP) for the weapon mesh
		_FirstPerson_SecondaryWeapon_SkeletalMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		_FirstPerson_SecondaryWeapon_SkeletalMesh->SetAnimInstanceClass(Weapon->GetAnimInstanceFirstPersonGun());

		// Play pickup animation
		uint8 handAnim = (uint8)E_HandAnimation::eHA_EquipDuelLeft;
		uint8 gunAnim = (uint8)0;
		OwningClient_PlaySecondaryWeaponFPAnimation(_FirstPerson_SecondaryWeapon_SkeletalMesh, 1.0f, false, true, handAnim, 0.0f, false, gunAnim, 0.0f);
	}	

	// Weapon == NULL
	else
	{
		// Nullify the animation BPs & skeletal meshes
		_FirstPerson_SecondaryWeapon_SkeletalMesh->SetAnimInstanceClass(NULL);
		_FirstPerson_SecondaryWeapon_SkeletalMesh->SetSkeletalMesh(NULL);
	}

	// Updating attachments for the weapon
	///UpdateFirstPersonSecondaryScopeAttachment(Weapon);
}

/*
*
*/
bool ABaseCharacter::Multicast_UpdateThirdPersonSecondaryWeaponMesh_Validate(AWeapon* Weapon)
{ return true; }

void ABaseCharacter::Multicast_UpdateThirdPersonSecondaryWeaponMesh_Implementation(AWeapon* Weapon)
{
	// Either update the mesh with the mesh referenced from the weapon, otherwise clear the mesh on the character
	if (Weapon != NULL)
	{
		// Set mesh
		_ThirdPerson_SecondaryWeapon_SkeletalMesh->SetSkeletalMesh(Weapon->GetThirdPersonMesh());

		// Set animation instance (anim BP)
		///GetMesh()->SetAnimInstanceClass(Weapon->GetAnimInstanceThirdPersonCharacter());
	}

	// Weapon == NULL
	else { _ThirdPerson_SecondaryWeapon_SkeletalMesh->SetSkeletalMesh(NULL); }
}

/*
*
*/
void ABaseCharacter::OnRep_SecondaryWeapon()
{
	// set _bDuelwielding's value based off _SecondaryWeapon's value
	bool bDuelWielding = _SecondaryWeapon != NULL ? true : false;
	if (GetLocalRole() == ROLE_Authority) { SetIsDuelWielding(bDuelWielding); }
	else { Server_Reliable_SetIsDuelWielding(bDuelWielding); }

	// Update meshes (DEPRECIATED) OnRep_DuelWielding calls these methods, which comes from the SetIsDuelWielding() function,
	// but I'm leaving the old method calls here just as a reminder

	///OwningClient_UpdateFirstPersonSecondaryWeaponMesh(_SecondaryWeapon); 
	///Multicast_UpdateThirdPersonSecondaryWeaponMesh(_SecondaryWeapon);
}

/*
*
*/
void ABaseCharacter::InitFireSecondaryWeapon()
{
	// Sanity check
	if (_SecondaryWeapon == NULL) { return; }
	if (_SecondaryWeapon->IsThereValidFireModeCount() == false) { return; }
	if (_SecondaryWeapon->GetCurrentFireMode() == NULL) { return; }

	if (_bIsReloadingSecondaryWeapon) { return; }
	if (_bIsTogglingWeapons) { return; }
	if (_bIsSprinting) { return; }

	// Weapon requires a full charge to fire
	if (_SecondaryWeapon->GetCurrentFireMode()->GetWeaponMustBeChargedToFire())
	{

	}

	// Weapon does NOT require charge to fire
	else
	{
		switch (_SecondaryWeapon->GetCurrentFireMode()->GetFiringType())
		{
		case E_FiringModeType::eFMT_FullAuto: FireWeaponTraceFullAuto(_SecondaryWeapon); break;
		case E_FiringModeType::eFMT_Burst: FireWeaponTraceBurst(_SecondaryWeapon); break;
		case E_FiringModeType::eFMT_SemiAuto: FireWeaponTraceSemiAuto(_SecondaryWeapon); break;
		case E_FiringModeType::eFMT_Spread: FireWeaponTraceSpread(_SecondaryWeapon); break;
		default: break;
		}
	}
}

/*
*
*/
void ABaseCharacter::InputSecondaryFirePress()
{
	// Sanity check
	if (_SecondaryWeapon == NULL) { return; }
	if (_SecondaryWeapon->IsThereValidFireModeCount() == false) { return; }
	if (_SecondaryWeapon->GetCurrentFireMode() == NULL) { return; }

	// Only true if the trigger type is press enter
	if (_SecondaryWeapon->GetCurrentFireMode()->GetFiringTriggerType() == E_FiringTriggerType::eFTT_Press)
	{
		_bIsTryingToFireSecondary = true;
		_bHasReleasedSecondaryTrigger = false;
	} else
	{
		_bIsTryingToFireSecondary = false;
		_bHasReleasedSecondaryTrigger = true;
	}
}

/*
*
*/
void ABaseCharacter::InputSecondaryFireRelease()
{
	// Sanity check
	if (_SecondaryWeapon == NULL) { return; }
	if (_SecondaryWeapon->IsThereValidFireModeCount() == false) { return; }
	if (_SecondaryWeapon->GetCurrentFireMode() == NULL) { return; }

	// Only true if the trigger type is press exit
	if (_SecondaryWeapon->GetCurrentFireMode()->GetFiringTriggerType() == E_FiringTriggerType::eFTT_Release)
	{
		_bIsTryingToFireSecondary = true;
		_bHasReleasedSecondaryTrigger = false;
	} else
	{
		_bIsTryingToFireSecondary = false;
		_bHasReleasedSecondaryTrigger = true;
	}
}

/*
*
*/
bool ABaseCharacter::OwningClient_Reliable_SecondaryWeaponCameraTrace_Validate()
{ return true; }

void ABaseCharacter::OwningClient_Reliable_SecondaryWeaponCameraTrace_Implementation()
{
	if (_SecondaryWeapon == NULL) { return; }

	// Get player controller ref
	APlayerController* ctrl = Cast<APlayerController>(GetController());

	// Get crosshair location in viewport (for now its just in the center of the screen, but will need some offset in the future)
	int viewportX, viewportY;
	ctrl->GetViewportSize(viewportX, viewportY);

	// Convert screen location to world space
	FVector worldLocation = FVector::ZeroVector;
	FVector worldDirection = FVector::ZeroVector;
	ctrl->DeprojectScreenPositionToWorld(viewportX / 2, viewportY / 2, worldLocation, worldDirection);

	// Get trace end point
	float traceLength = _SecondaryWeapon->GetCurrentFireMode()->GetMaxRangeThreshold();
	FVector spreadPoint = FMath::VRandCone(worldDirection, (_SecondaryWeapon->GetCurrentFireMode()->GetCurrentProjectileSpread() * GetWorld()->GetDeltaSeconds()) / 2);
	FVector traceEnd = FVector::ZeroVector;
	traceEnd = worldLocation + (worldDirection + (spreadPoint * traceLength));

	// Fire line trace
	FHitResult hitResult;
	FCollisionQueryParams params;
	params.bTraceComplex = false;
	params.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(hitResult, worldLocation, traceEnd, ECollisionChannel::ECC_Camera, params);

	if (_bDebugDrawSecondaryWeaponCameraTrace)
	{
		DrawDebugLine(GetWorld(), worldLocation, traceEnd, _fSecondaryWeaponCameraTraceColour, false, 1.0f);
	}

	Server_Reliable_SecondaryWeaponCameraTrace(hitResult);
}

/*
*
*/
bool ABaseCharacter::Server_Reliable_SecondaryWeaponCameraTrace_Validate(FHitResult ClientHitResult)
{ return true; }

void ABaseCharacter::Server_Reliable_SecondaryWeaponCameraTrace_Implementation(FHitResult ClientHitResult)
{
	if (_FirstPerson_SecondaryWeapon_SkeletalMesh == NULL) { return; }

	if (ClientHitResult.bBlockingHit)
	{
		// Get projectile transform
		FTransform trans = FTransform::Identity;
		FVector charMuzzle = _FirstPerson_SecondaryWeapon_SkeletalMesh->GetSocketLocation("MuzzleLaunchPoint");
		FVector v = FVector(ClientHitResult.ImpactPoint - charMuzzle);
		UKismetMathLibrary::Vector_Normalize(v, 0.0001);
		FRotator rot = UKismetMathLibrary::MakeRotFromX(v);
		FQuat quat = FQuat(rot);
		trans.SetLocation(charMuzzle);
		trans.SetRotation(quat);

		FVector cameraForwardXVector = trans.GetRotation().Vector();

		// Start secondary weapon fire
		_SecondaryWeapon->GetCurrentFireMode()->Fire(ClientHitResult, cameraForwardXVector, _FirstPerson_SecondaryWeapon_SkeletalMesh, _ThirdPerson_SecondaryWeapon_SkeletalMesh);
	}

	// !ClientHitResult.bBlockingHit
	else
	{
		// Get projectile transform
		FTransform trans = FTransform::Identity;
		FVector charMuzzle = _FirstPerson_SecondaryWeapon_SkeletalMesh->GetSocketLocation("MuzzleLaunchPoint");
		FVector v = FVector(ClientHitResult.TraceEnd - charMuzzle);
		UKismetMathLibrary::Vector_Normalize(v, 0.0001);
		FRotator rot = UKismetMathLibrary::MakeRotFromX(v);
		FQuat quat = FQuat(rot);
		trans.SetLocation(charMuzzle);
		trans.SetRotation(quat);

		FVector cameraForwardXVector = trans.GetRotation().Vector();

		// Start secondary weapon fire
		_SecondaryWeapon->GetCurrentFireMode()->Fire(ClientHitResult, cameraForwardXVector, _FirstPerson_SecondaryWeapon_SkeletalMesh, _ThirdPerson_SecondaryWeapon_SkeletalMesh);
	}
}

/**
* @summary:	Checks and initiates a reload of the character's secondary weapon.
*
* @return:	virtual void
*/
void ABaseCharacter::InputReloadSecondaryWeapon()
{
	// Sanity check
	if (_SecondaryWeapon == NULL) { return; }
	if (!_SecondaryWeapon->IsThereValidFireModeCount()) { return; }
	if (_SecondaryWeapon->GetCurrentFireMode() == NULL) { return; }
	UFireMode* fireMode = _SecondaryWeapon->GetCurrentFireMode();
	if (fireMode == NULL) { return; }
	UAmmo* ammoPool = fireMode->GetAmmoPool();
	if (ammoPool == NULL) { return; }

	// Don't continue if we're already reloading
	if (_bIsReloadingSecondaryWeapon) { return; }
	_bSecondaryReloadCancelled = false;

	// is duel wielding? (we can only reload our secondary if we're duel wielding)
	if (_bIsDuelWielding)
	{
		switch (fireMode->GetReloadType())
		{
		case E_ReloadType::eRT_Magazine:
		{
			// Doesn't have ammo in reserves? Don't progress
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
						// Dont play an animation because we're dual wielding.. instead just insta-chamber the round then continue on
						fireMode->Server_Reliable_ChamberRound();
					}
				}
			}

			// Magazine full? Don't progress
			if (ammoPool->GetMagazineAmmo() >= ammoPool->GetMaximumMagazineAmmo()) { return; }

			// Cannot shoot whilst we are reloading
			_bCanFireSecondary = false;

			// Start reloading state/sequence
			Server_Reliable_SetIsReloadingSecondaryWeapon(true);

			// Get reload lower montage references for playing
			uint8 handAnimByte; handAnimByte = (uint8)E_HandAnimation::eHA_ReloadDuelLeftLower;
			uint8 gunAnimByte; gunAnimByte = (uint8)E_GunAnimation::eGA_ReloadDuelLeftLower;

			// Play reload (lower) animation
			float startingFrame = _SecondaryWeapon->GetCurrentFireMode()->GetReloadStartingTime();
			OwningClient_PlaySecondaryWeaponFPAnimation(_FirstPerson_ArmsDuelLeft, _fGlobalReloadPlayRate, true, true, handAnimByte, startingFrame, true, gunAnimByte, startingFrame);

			// Get timer duration (using a timer for the weapon raise animation)
			UAnimMontage* lowerMontage = _SecondaryWeapon->GetArmAnimation((E_HandAnimation)handAnimByte);
			float lowerAnimLength = lowerMontage->GetPlayLength();
			float reloadTime = _SecondaryWeapon->GetDuelWieldingReloadTime();

			// Start delay timer for the weapon raise
			FTimerDelegate raiseDelegate;
			raiseDelegate.BindUFunction(this, FName("OnDuelWielding_SecondaryReloadComplete"));
			GetWorld()->GetTimerManager().SetTimer(_fDuelSecondaryReloadHandle, raiseDelegate, 1.0f, false, lowerAnimLength + reloadTime);

			break;
		}
		case E_ReloadType::eRT_LoadSingle: { break; }
		default: break;
		}
	}
}

/*
*
*/
void ABaseCharacter::OnDuelWielding_SecondaryReloadComplete()
{
	uint8 handAnimByte; handAnimByte = (uint8)E_HandAnimation::eHA_ReloadDuelLeftRaise;
	uint8 gunAnimByte; gunAnimByte = (uint8)E_GunAnimation::eGA_ReloadDuelLeftRaise;

	// Play reload (raise) animation
	float startingFrame = 0.0f;
	OwningClient_PlaySecondaryWeaponFPAnimation(_FirstPerson_ArmsDuelLeft, _fGlobalReloadPlayRate, false, true, handAnimByte, startingFrame, true, gunAnimByte, startingFrame);
}

/*
*
*/
bool ABaseCharacter::OwningClient_PlaySecondaryWeaponFPAnimation_Validate(USkeletalMeshComponent* ArmsMesh, float PlayRate, bool FreezeMontageAtLastFrame, bool PlayHandAnimation, uint8 HandAnimation, float HandAnimationStartingFrame, bool PlayGunAnimation, uint8 GunAnimation, float GunAnimationStartingFrame)
{ return true; }

void ABaseCharacter::OwningClient_PlaySecondaryWeaponFPAnimation_Implementation(USkeletalMeshComponent* ArmsMesh, float PlayRate, bool FreezeMontageAtLastFrame, bool PlayHandAnimation, uint8 HandAnimation, float HandAnimationStartingFrame, bool PlayGunAnimation, uint8 GunAnimation, float GunAnimationStartingFrame)
{
	PlaySecondaryWeaponFPAnimation(ArmsMesh, PlayRate, FreezeMontageAtLastFrame, PlayHandAnimation, HandAnimation, HandAnimationStartingFrame, PlayGunAnimation, GunAnimation, GunAnimationStartingFrame);
}

/*
*
*/
void ABaseCharacter::PlaySecondaryWeaponFPAnimation(USkeletalMeshComponent* ArmsMesh, float PlayRate, bool FreezeMontageAtLastFrame, bool PlayHandAnimation, uint8 HandAnimation, float HandAnimationStartingFrame, bool PlayGunAnimation, uint8 GunAnimation, float GunAnimationStartingFrame)
{
	// Sanity checks
	if (_SecondaryWeapon == NULL) { return; }

	// arm animation play
	float i = 0.05f;
	if (PlayHandAnimation)
	{
		UAnimMontage* handMontage = _SecondaryWeapon->GetArmAnimation((E_HandAnimation)HandAnimation);
		UAnimInstance* handAnimInstance = _FirstPerson_ArmsDuelLeft->GetAnimInstance();
		if (handMontage != NULL && handAnimInstance != NULL)
		{
			// Play hand montage
			ArmsMesh->SetHiddenInGame(false, true); // Force unhide the mesh before playing the animations
			handAnimInstance->Montage_Play(handMontage, PlayRate, EMontagePlayReturnType::MontageLength, HandAnimationStartingFrame, true);
			if (FreezeMontageAtLastFrame)
			{
				// Start delay timer then freeze anim
				float playLength = handMontage->GetPlayLength();
				FTimerDelegate animationDelegate;
				animationDelegate.BindUFunction(this, FName("FreezeAnimation"), ArmsMesh, handMontage, playLength);
				GetWorld()->GetTimerManager().SetTimer(_fSecondaryFPAnimationHandle, animationDelegate, 1.0f, false, (playLength / _fGlobalReloadPlayRate) - i);
			}
		}
	}

	// weapon animation play
	if (PlayGunAnimation)
	{
		UAnimMontage* gunMontage = _SecondaryWeapon->GetGunAnimation((E_GunAnimation)GunAnimation);
		UAnimInstance* weaponAnimInstance = _FirstPerson_SecondaryWeapon_SkeletalMesh->GetAnimInstance();
		if (gunMontage != NULL && weaponAnimInstance != NULL)
		{
			// Play gun montage
			_FirstPerson_SecondaryWeapon_SkeletalMesh->SetHiddenInGame(false); // Force unhide the mesh before playing the animations
			weaponAnimInstance->Montage_Play(gunMontage, PlayRate, EMontagePlayReturnType::MontageLength, GunAnimationStartingFrame, true);
			if (FreezeMontageAtLastFrame)
			{
				// Start delay timer then freeze anim
				float playLength = gunMontage->GetPlayLength();
				FTimerDelegate animationDelegate;
				animationDelegate.BindUFunction(this, FName("FreezeAnimation"), _FirstPerson_SecondaryWeapon_SkeletalMesh, gunMontage, playLength);
				GetWorld()->GetTimerManager().SetTimer(_fSecondaryFPAnimationHandle, animationDelegate, 1.0f, false, (playLength / _fGlobalReloadPlayRate) - i);
			}
		}
	}
}

/*
*
*/
bool ABaseCharacter::Server_Reliable_SetIsReloadingSecondaryWeapon_Validate(bool ReloadingSecondary)
{ return true; }

void ABaseCharacter::Server_Reliable_SetIsReloadingSecondaryWeapon_Implementation(bool ReloadingSecondary)
{
	_bIsReloadingSecondaryWeapon = ReloadingSecondary;
}

/*
*
*/
bool ABaseCharacter::Server_Reliable_SetIsDuelWielding_Validate(bool bDuelWielding)
{ return true; }

void ABaseCharacter::Server_Reliable_SetIsDuelWielding_Implementation(bool bDuelWielding)
{
	SetIsDuelWielding(bDuelWielding);
}

/*
*
*/
void ABaseCharacter::SetIsDuelWielding(bool bDuelWielding)
{
	_bIsDuelWielding = bDuelWielding;
	if (GetLocalRole() == ROLE_Authority) { OnRep_DuelWielding(); }
}

/*
*
*/
void ABaseCharacter::OnRep_DuelWielding()
{
	// Update meshes
	OwningClient_UpdateFirstPersonSecondaryWeaponMesh(_SecondaryWeapon);
	Multicast_UpdateThirdPersonSecondaryWeaponMesh(_SecondaryWeapon);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Inventory | Weapon | Reserve

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
{ return true; }

void ABaseCharacter::Server_Reliable_SetReserveWeapon_Implementation(AWeapon* Weapon)
{
	_ReserveWeapon = Weapon;

	// Set weapon owner
	_ReserveWeapon->SetOwner(this);
	_ReserveWeapon->SetNewOwner(this);
	_ReserveWeapon->SetOwnersPrimaryWeapon(false);
	_ReserveWeapon->SetOwnersSecondaryWeapon(false);

	if (GetLocalRole() == ROLE_Authority) { OnRep_ReserveWeapon(); }
}

/*
*
*/
void ABaseCharacter::OnRep_ReserveWeapon()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Base

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

bool ABaseCharacter::Server_Reliable_SetMovingSpeed_Validate(float Speed)
{ return GetCharacterMovement() != NULL; }

void ABaseCharacter::Server_Reliable_SetMovingSpeed_Implementation(float Speed)
{ Multicast_Reliable_SetMovingSpeed(Speed); }

bool ABaseCharacter::Multicast_Reliable_SetMovingSpeed_Validate(float Speed)
{ return GetCharacterMovement() != NULL; }

void ABaseCharacter::Multicast_Reliable_SetMovingSpeed_Implementation(float Speed)
{ GetCharacterMovement()->MaxWalkSpeed = Speed; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Crouch

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void ABaseCharacter::EnterCrouch()
{
	/*
		// Cannot crouch when airborne
		if (!GetCharacterMovement()->IsMovingOnGround()) { return; }

		// Lerp camera transform?
		_bCrouchEnter = true;
		if (_fCrouchCameraLerpTime >= _fCrouchLerpingDuration) { _fCrouchCameraLerpTime = 0.0f; }
		else
		{
			// Get current percent of lerp time so that it doesn't jagger at the start of the next lerp sequence
			float t = _fCrouchCameraLerpTime;
			_fCrouchCameraLerpTime = _fCrouchLerpingDuration - t;
		}
		if (_PrimaryWeapon != NULL)
		{
			if (_PrimaryWeapon->GetCurrentFireMode()->IsAimDownSightEnabled())
			{ _bLerpCrouchCamera = !IsAiming(); }
			else
			{ _bLerpCrouchCamera = true; }
		}

		// _PrimaryWeapon == NULL
		else { _bLerpCrouchCamera = true; }

		// Set _bIsCrouching = TRUE
		if (Role == ROLE_Authority) { _bIsCrouching = true; } else { Server_Reliable_SetIsCrouching(true); }
	*/
}

void ABaseCharacter::ExitCrouch()
{
	/*
		// Can only stop crouching if we we're previously crouching TRUE????
		if (_bIsCrouching)
		{
		// Lerp camera transform?
			_bCrouchEnter = false;
			if (_fCrouchCameraLerpTime >= _fCrouchLerpingDuration) { _fCrouchCameraLerpTime = 0.0f; }
			else
			{
				// Get current percent of lerp time so that it doesn't jagger at the start of the next lerp sequence
				float t = _fCrouchCameraLerpTime;
				_fCrouchCameraLerpTime = _fCrouchLerpingDuration - t;
			}
			if (_PrimaryWeapon != NULL)
		{
			if (_PrimaryWeapon->GetCurrentFireMode()->IsAimDownSightEnabled())
			{ _bLerpCrouchCamera = !IsAiming(); }
			else
			{ _bLerpCrouchCamera = true; }
		}

			// _PrimaryWeapon == NULL
			else { _bLerpCrouchCamera = true; }

			// Set _bIsCrouching = FALSE
			if (Role == ROLE_Authority) { _bIsCrouching = false; } else { Server_Reliable_SetIsCrouching(false); }
		}
	*/
}

bool ABaseCharacter::Server_Reliable_SetIsCrouching_Validate(bool IsCrouching)
{ return true; }

void ABaseCharacter::Server_Reliable_SetIsCrouching_Implementation(bool IsCrouching)
{
	_bIsCrouching = IsCrouching;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Jump

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

					_bIsPerformingGroundChecks = true;
				}
			}
		}
	} else { return; }
}

bool ABaseCharacter::Server_Reliable_SetJumping_Validate(bool Jumping)
{ return true; }

void ABaseCharacter::Server_Reliable_SetJumping_Implementation(bool Jumping)
{
	_bIsJumping = Jumping;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Sprint

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

/*
*
*/
void ABaseCharacter::SprintEnter()
{
	if (_bSprintEnabled)
	{
		if (!_bIsAiming)
		{ _bTryingToSprint = true; }
	}
}

/*
*
*/
void ABaseCharacter::SprintExit()
{
	if (!_bIsAiming) { _bTryingToSprint = false; }
}

/*
*
*/
void ABaseCharacter::SprintToggle(bool Sprint)
{
	// Sprint enter
	if (Sprint)
	{
		if (!_bIsSprinting) { SprintEnter(); }
	}

	// Sprint exit
	else
	{ SprintExit(); }
}

/*
*
*/
void ABaseCharacter::Tick_Sprint()
{
	UCharacterMovementComponent* characterMovement = GetCharacterMovement();
	if (characterMovement == NULL) { return; }

	// Check if forward input is being pressed in this frame
	bool forward = false;
	auto ctrl = Cast<ABasePlayerController>(this->GetController()); if (ctrl == NULL) { return; }

	if (ctrl->GetControllerType() == E_ControllerType::eCT_Keyboard) { forward = ctrl->IsInputKeyDown(EKeys::W); }
	else if (ctrl->GetControllerType() == E_ControllerType::eCT_Xbox) { forward = ctrl->IsInputKeyDown(EKeys::Gamepad_LeftStick_Up); }
	else if (ctrl->GetControllerType() == E_ControllerType::eCT_PlayStation) { forward = ctrl->IsInputKeyDown(EKeys::Gamepad_LeftStick_Up); }

	// This is so that when the forward input is released, the sprint is forcibly stopped.
	else { StopSprinting(); }

	// Forward input is present this frame
	if (forward)
	{
		// Moving on the ground
		if (characterMovement->IsMovingOnGround())
		{
			// Not aiming
			if (_bIsAiming) { AimWeaponExit(); }

			// Set sprinting = true
			if (!_bIsSprinting)
			{
				// Set _bIsSprinting on server for replication
				if (HasAuthority())
				{ _bIsSprinting = true; } else
				{ Server_Reliable_SetSprinting(true); }

				_fOnSprintEnter.Broadcast(true);
			}

			// Set sprinting movement speed
			if (characterMovement->GetMaxSpeed() != _fMovementSpeedSprint)
			{
				if (HasAuthority()) { Multicast_Reliable_SetMovingSpeed(_fMovementSpeedSprint); } 
				else { Server_Reliable_SetMovingSpeed(_fMovementSpeedSprint); }
			}

			// Play feedback(s) [Camera shakes / Gamepad Rumbles]
			OwningClient_PlayCameraShake(_CameraShakeSprint, 1.0f);
		}

		// In the air
		else { StopSprinting(); }
	}

	// Not pressing forward input anymore, stop sprinting bruh
	else { StopSprinting(); }
}

/*
*
*/
void ABaseCharacter::StopSprinting()
{
	UCharacterMovementComponent* characterMovement = GetCharacterMovement();
	if (characterMovement == NULL) { return; }

	// Set sprinting = false
	if (_bIsSprinting)
	{
		// Set _bIsSprinting on server for replication
		if (HasAuthority())
		{ _bIsSprinting = false; } else
		{ Server_Reliable_SetSprinting(false); }

		_fOnSprintEnter.Broadcast(false);
	}

	// Reset movement speed	once the character touches the floor
	float movementSpeed = _fMovementSpeedJog; // This needs to determine if we should go to aim/crouch/walk/jog movement speed
	if (characterMovement->GetMaxSpeed() != movementSpeed)
	{
		if (HasAuthority())
		{ Multicast_Reliable_SetMovingSpeed(movementSpeed); } else
		{ Server_Reliable_SetMovingSpeed(movementSpeed); }
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Movement | Vault

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	if (UKismetMathLibrary::NotEqual_VectorVector(_vWallHeightLocation, FVector(0.0f, 0.0f, 0.0f), 10.0f) && _bCanVault)
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
			USkeletalMeshComponent* armsMesh = !_bIsDuelWielding ? _FirstPerson_Arms : _FirstPerson_ArmsDuelRight;
			OwningClient_PlayPrimaryWeaponFPAnimation(armsMesh, 1.0f, false, true, byteHandAnim, 0.f, false, byteGunAnim, 0.0f);

			// Grab ledge (on ALL clients)
			FVector moveLocation = GetMoveToLocation(_fVaultHeightOffset, _fVaultForwardOffset);
			if (GetLocalRole() == ROLE_Authority)
			{ Multicast_Reliable_GrabLedge(moveLocation); }
			else 
			{ Server_Reliable_GrabLedge(moveLocation); }

			// Set IsCliming/Vaulting
			if (GetLocalRole() == ROLE_Authority)
			{ _bIsVaulting = true; } 
			else
			{ Server_Reliable_SetIsVaulting(true); }
		}
	}
}

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

/*
*
*/
bool ABaseCharacter::Server_Reliable_GrabLedge_Validate(FVector MoveLocation)
{ return true; }

void ABaseCharacter::Server_Reliable_GrabLedge_Implementation(FVector MoveLocation)
{
	Multicast_Reliable_GrabLedge(MoveLocation);
}

/*
*
*/
bool ABaseCharacter::Multicast_Reliable_GrabLedge_Validate(FVector MoveLocation)
{ return true; }

void ABaseCharacter::Multicast_Reliable_GrabLedge_Implementation(FVector MoveLocation)
{
	GrabLedge(MoveLocation);
}

/*
*
*/
void ABaseCharacter::ClimbLedge()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	// No longer climbing/vaulting
	if (GetLocalRole() == ROLE_Authority)
	{ _bIsVaulting = false; }
	else
	{ Server_Reliable_SetIsVaulting(false); }

	_bIsTryingToVault = false;
}

bool ABaseCharacter::Server_Reliable_SetIsVaulting_Validate(bool Vaulting)
{ return true; }

void ABaseCharacter::Server_Reliable_SetIsVaulting_Implementation(bool Vaulting)
{
	_bIsVaulting = Vaulting;
}

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Stamina

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
