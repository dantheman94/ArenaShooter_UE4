// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Character.h"
#include "Structures.h"

#include "BaseCharacter.generated.h"

// *** DEFINITIONS

#define _MAX_FLESH_HEALTH	100.0f
#define _MAX_SHIELD			100.0f

// *** ENUMERATORS

UENUM(BlueprintType)
enum class E_AimDirection : uint8
{
	ePT_ZoomIn UMETA(DisplayName = "ZoomIn"),
	ePT_ZoomOut UMETA(DisplayName = "ZoomOut")
};

UENUM(BlueprintType)
enum class E_CharacterStates : uint8
{
	eCS_Idle UMETA(DisplayName = "Idle"),
	eCS_Walking UMETA(DisplayName = "Walking"),
	eCS_Jogging UMETA(DisplayName = "Joggin"),
	eCS_Sprinting UMETA(DisplayName = "Sprinting"),
	eCS_Crouching UMETA(DisplayName = "Crouching"),
	eCS_Sliding UMETA(DisplayName = "Sliding"),
	eCS_Jumping UMETA(DisplayName = "Jumping"),
	eCS_DoubleJumping UMETA(DisplayName = "DoubleJumping"),
	eCS_Dashing UMETA(DisplayName = "Dashing"),
	eCS_Hovering UMETA(DisplayName = "Hovering"),
	eCS_Montage UMETA(DisplayName = "Playing Montage"),
	eCS_Ragdoll UMETA(DisplayName = "Ragdolling")
};

UENUM(BlueprintType)
enum class E_CombatStates : uint8
{
	ePT_Aiming UMETA(DisplayName = "ZoomIn"),
	ePT_ZoomOut UMETA(DisplayName = "ZoomOut")
};

// *** EVENT DISPATCHERS / DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FADSAnimDelegate, bool, AimingEnter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTakeDamage, float, DamageAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSprintEnter, bool, IsSprinting);

// *** CLASSES

class UCameraShake;
class AController;
class AWeapon;
class UCameraComponent;
class USpringArmComponent;
class UStamina;
class UFireMode;
class AInteractable;

UCLASS()
class ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Public Variables

public:

	// Character | FirstPerson ****************************************************************************************************************

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UCameraComponent* _FirstPerson_Camera = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USpringArmComponent* _FirstPerson_SpringArm = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USkeletalMeshComponent* _FirstPerson_Arms = NULL;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USkeletalMeshComponent* _FirstPerson_ArmsDuelLeft = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USkeletalMeshComponent* _FirstPerson_ArmsDuelRight = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USkeletalMeshComponent* _FirstPerson_PrimaryWeapon_SkeletalMesh = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USkeletalMeshComponent* _FirstPerson_SecondaryWeapon_SkeletalMesh = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) 
		UStaticMeshComponent* _FirstPerson_PrimaryWeapon_Sight_StaticMesh = NULL;

	// Character | ThirdPerson ****************************************************************************************************************

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USkeletalMeshComponent* _ThirdPerson_PrimaryWeapon_SkeletalMesh = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USkeletalMeshComponent* _ThirdPerson_SecondaryWeapon_SkeletalMesh = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USkeletalMeshComponent* _ThirdPerson_ReserveWeapon_SkeletalMesh = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UCapsuleComponent* _HitBox_Head = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UCapsuleComponent* _HitBox_TorsoUpper = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UCapsuleComponent* _HitBox_TorsoLower = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UCapsuleComponent* _HitBox_RightShoulder = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UCapsuleComponent* _HitBox_RightForearm = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UCapsuleComponent* _HitBox_RightHand = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UCapsuleComponent* _HitBox_LeftShoulder = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UCapsuleComponent* _HitBox_LeftForearm = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UCapsuleComponent* _HitBox_LeftHand = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UCapsuleComponent* _HitBox_RightThigh = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UCapsuleComponent* _HitBox_RightShin = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UCapsuleComponent* _HitBox_LeftThigh = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UCapsuleComponent* _HitBox_LeftShin = NULL;

#pragma endregion Public Variables

#pragma region Protected Variables
		
protected:

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Startup 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Default Settings")
		float _fDefaultCapsuleHalfHeight = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Default Settings")
		float _fCameraRotationLagSpeed = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Default Settings")
		float _fDefaultGravityScale = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Default Settings")
		float _fDefaultAirControl = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Default Settings")
		float _fDefaultGroundFriction = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Default Settings")
		float _fDefaultBrakingFrictionFactor = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Default Settings")
		float _fDefaultBrakingDecelerationWalking = 0.0f;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Current Frame 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool _bIsPerformingGroundChecks = false;
	float _fFallingVelocity = 0.0f;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Animation

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		float _fGlobalTogglePlayRate = 2.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		float _fGlobalReloadPlayRate = 1.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Animation")
		UAnimMontage* _CurrentMontagePlaying = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation")
		bool _bCancelAnimation = false;

	/*
	*	A timer handle used for referencing the primary (right arm) animation.
	*/
	UPROPERTY()
		FTimerHandle _fPrimaryFPAnimationHandle;

	/*
	*	A timer handle used for referencing the secondary (left arm) animation.
	*/
	UPROPERTY()
		FTimerHandle _fSecondaryFPAnimationHandle;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Controller | Input 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*	A scalar value (0 - 1 FLOAT) that represents the amount of input being driven into the forward/backward input (REPLICATED).
	*	EG: Keyboard will either be -1.0, 0.0 or 1.0 explicitly, but a gamepad could be somewhere in between this range due to its axis controller).
	*	This is primarily used for animation (Blending from idle ->  walk -> jog animation state)
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Controller | Input", Replicated)
		float _fForwardInputScale = 0.0f;
	
	/*
	*	A scalar value (0 - 1 FLOAT) that represents the amount of input being driven into the right/left input (REPLICATED).
	*	EG: Keyboard will either be -1.0, 0.0 or 1.0 explicitly, but a gamepad could be somewhere in between this range due to its axis controller).
	*	This is primarily used for animation (Blending from idle ->  walk -> jog animation state)
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Controller | Input", Replicated)
		float _fRightInputScale = 0.0f;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Health | 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*	Returns whether the character has HEALTH greater than 0.0f (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Health", Replicated)
		bool _bIsAlive = false;

	/*
	*	Returns whether the character is currently taking damage or not (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Health", Replicated)
		bool _bIsTakingDamage = false;

	/*
	*	The amount of time (in seconds) that must past since the last frame that the character was inflicted with damage,
	*	before the _bIsTakingDamage is reset back to FALSE.
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Health")
		float _fTakingDamageResetDelay = 0.2f;

	/*
	*	A list of controllers that have inflicted damage to the character since they were at a maximum health & shield value.
	*	This list is cleared only once the character has reached its maximum health values again.
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Health")
		TArray<AController*>_DamageInstigators;

	/*
	*
	*/
	UPROPERTY(BlueprintAssignable)
		FOnTakeDamage _fOnTakeDamage;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Health | Burn 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Health | Burn", Replicated)
		bool _bIsBurning = false;

	/*
	*	A timer handle used for referencing the burn delay on this character.
	*/
	UPROPERTY()
		FTimerHandle _fBurnDelayHandle;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Health | Flesh 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*	The current health value of the character (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Health | Flesh", Replicated)
		float _fFleshHealth = _MAX_FLESH_HEALTH;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Health | Shield 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*	The minimum delay of time (in seconds) that must pass since the last frame that the character was inflicted with damage,
	*	before the shields can begin recharging.
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Health | Shield")
		float _fShieldRechargeDelay = 3.0f;

	/*
	*	The rate of recharge of the shield when it is below its maximum value.
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Health | Shield")
		float _fShieldRechargingRate = 40.0f;

	/*
	*	The current shield value of the character (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Health | Shield", Replicated)
		float _fShield = _MAX_SHIELD;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Health | Shield")
		bool _bRechargeShields = false;

	/*
	*	A timer handle used for referencing the shield recharging delay.
	*/
	UPROPERTY()
		FTimerHandle _fShieldRechargeDelayHandle;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Interaction 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Interaction")
		TArray<UInteractionDataComponent*> _Interactables;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Interaction")
		UInteractionDataComponent* _FocusInteractable = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Interaction")
		float _fInteractionThresholdTime = 2.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Interaction")
		bool _bIsTryingToInteractPrimary = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Interaction")
		bool _bIsTryingToInteractSecondary = false;

	/*
	*
	*/
	UPROPERTY()
		FTimerHandle _fInteractionHandle;

	/*
	*
	*/
	UPROPERTY()
		FTimerHandle _fSetWeaponHandle;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Starting 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*	Returns whether the starting loadout of this character should use the loadout defined in the current gamemode.
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory | Starting")
		bool _bUseStartingLoadoutFromGamemode = true;

	/*
	*	The starting primary weapon of the character when created.
	*	(Only valid if _UseStartingLoadoutFromGamemode is FALSE).
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory | Starting")
		TSubclassOf<class AWeapon> _StartingPrimaryWeaponClass = NULL;

	/*
	*	The starting secondary weapon of the character when created.
	*	(Only valid if _UseStartingLoadoutFromGamemode is FALSE AND IF the _StartedPrimaryWeaponClass is a _bDuelWieldable AWeapon).
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory | Starting")
		TSubclassOf<class AWeapon> _StartingSecondaryWeaponClass = NULL;

	/*
	*	The starting reserve weapon of the character when created (Holstered weapon).
	*	(Only valid if _UseStartingLoadoutFromGamemode is FALSE).
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory | Starting")
		TSubclassOf<class AWeapon> _StartingReserveWeaponClass = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Inventory | Starting")
		bool _bStartingLoadout = true;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Weapon | Aiming 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory | Weapon | Aiming")
		FName _AimSocketName = TEXT("AimingCameraPoint");

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory | Weapon | Aiming")
		float _fAimingSpringArmRotationLag = 40.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Aiming")
		bool _bCanAim = true;

	/*
	*	Returns whether the character is currently aiming (ADS) their weapon or not (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Aiming", Replicated)
		bool _bIsAiming = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Aiming")
		float _fAimTime = 0.0f;

	/*
	*	
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Aiming")
		int _iCurrentFovStage = 0;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Aiming")
		bool _bIsAimLerping = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Aiming")
		float _fFovStart = 90.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Aiming")
		float _fFovTarget = 90.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Aiming")
		FTransform _TargetOriginTransform = FTransform::Identity;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Aiming")
		FTransform _StartingOriginTransform = FTransform::Identity;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Aiming")
		FVector _FpsArmOffset = FVector::ZeroVector;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Aiming")
		E_AimDirection _eAimDirection = E_AimDirection::ePT_ZoomIn;

	/*
	*
	*/
	UPROPERTY(BlueprintAssignable, Category = "Inventory: Weapon | Aiming")
		FADSAnimDelegate _fAdsAnimationEvent;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Weapon | Toggle

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*	A timer handle used for referencing the actual toggle.
	*/
	UPROPERTY()
		FTimerHandle _fToggleHandle;

	/*
	*	A timer handle used for referencing the toggle exit.
	*/
	UPROPERTY()
		FTimerHandle _fExitToggleHandle;

	/*
	*	Returns reference if the character is currently tabbing between its inventory weapons or not (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Tabbing", Replicated)
		bool _bIsTogglingWeapons = false;

	/*
	*	Returns reference if the character is currently equipping a new inventory weapon or not (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Tabbing", Replicated)
		bool _bIsEquippingNewWeapon = false;

	/*
	*	Returns reference to the previous primary weapon of the character (Used for toggling weapons) (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Tabbing", Replicated)
		AWeapon* _OldPrimaryWeapon = NULL;

	/*
	*	Returns reference to the previous reserve weapon of the character (Used for toggling weapons) (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Tabbing", Replicated)
		AWeapon* _OldReserveWeapon = NULL;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Weapon | Primary

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*	Returns reference to the current primary weapon of the character (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Primary", ReplicatedUsing = OnRep_PrimaryWeapon)
		AWeapon* _PrimaryWeapon = NULL;

	/*
	*	Returns if whether the character is currently reloading their primary weapon or not (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Primary", Replicated)
		bool _bIsReloadingPrimaryWeapon = false;

	/*
	*	Returns if the character has cancelled reloading their primary weapon or not.
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Primary")
		bool _bPrimaryReloadCancelled = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Primary")
		bool _bCanFirePrimary = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Primary")
		bool _bIsTryingToFirePrimary = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Primary")
		bool _bHasReleasedPrimaryTrigger = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory | Weapon | Primary")
		bool _bDebugDrawPrimaryWeaponCameraTrace = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory | Weapon | Primary",
		meta = (EditCondition = "_bDebugDrawPrimaryWeaponCameraTrace"))
		FColor _fPrimaryWeaponCameraTraceColour = FColor::Red;

	/*
	*	A timer handle used for referencing the primary weapon reloading timer (NOT DUEL WIELDING)
	*/
	UPROPERTY()
		FTimerHandle _fPrimaryReloadHandle;

	/*
	*	A timer handle used for referencing the primary weapon reloading timer
	*/
	UPROPERTY()
		FTimerHandle _fDuelPrimaryReloadHandle;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Weapon | Secondary 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*	Returns reference to the current secondary weapon of the character (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Secondary", ReplicatedUsing = OnRep_DuelWielding)
		AWeapon* _SecondaryWeapon = NULL;

	/*
	*	Returns whether the character is duel wielding a secondary weapon or not (REPLICATED).
	*	This should be inline if whether _SecondaryWeapon isValid or not.
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Secondary", Replicated)
		bool _bIsDuelWielding = false;

	/*
	*	Returns if whether the character is currently reloading their secondary weapon or not (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Secondary", Replicated)
		bool _bIsReloadingSecondaryWeapon = false;

	/*
	*	Returns if the character has cancelled reloading their secondary weapon or not.
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Secondary")
		bool _bSecondaryReloadCancelled = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Secondary")
		bool _bCanFireSecondary = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Secondary")
		bool _bIsTryingToFireSecondary = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Secondary")
		bool _bHasReleasedSecondaryTrigger = true;

	/*
	*	
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory | Weapon | Secondary")
		bool _bDuelWieldingIsMirrored = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory | Weapon | Secondary")
		bool _bDebugDrawSecondaryWeaponCameraTrace = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory | Weapon | Secondary",
		meta = (EditCondition = "_bDebugDrawSecondaryWeaponCameraTrace"))
		FColor _fSecondaryWeaponCameraTraceColour = FColor::Red;

	/*
	*	A timer handle used for referencing the secondary weapon reloading timer.
	*/
	UPROPERTY()
		FTimerHandle _fDuelSecondaryReloadHandle;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Weapon | Reserve 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*	Returns reference to the current reserve weapon of the character (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Weapon | Reserve", ReplicatedUsing = OnRep_ReserveWeapon)
		AWeapon* _ReserveWeapon = NULL;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Grenade 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory | Grenade", Replicated)
		int _iFragmentationGrenadeCount = 0;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory | Grenade", Replicated)
		int _iEMPGrenadeCount = 0;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory | Grenade", Replicated)
		int _iIncendiaryGrenadeCount = 0;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory | Grenade")
		int _iMaximumGrenadeCount = 2;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Inventory | Grenade")
		E_GrenadeType _eCurrentGrenadeType = E_GrenadeType::eWBT_Frag;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | Crouching 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Crouch")
		float _fCrouchLerpingDuration = 0.25f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Crouch")
		float _fCrouchCameraLerpTime = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Crouch")
		float _fCrouchCapsuleLerpTime = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Crouch")
		bool _bLerpCrouchCamera = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Crouch")
		bool _bLerpCrouchCapsule = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Crouch")
		bool _bCrouchEnter = false;

	/*
	*	Returns whether the character is currently in a crouching state or not (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Crouch", Replicated)
		bool _bIsCrouching = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Crouch")
		FTransform _tCrouchWeaponOrigin;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | Jog 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*	A scalar multiplier of the camera shake used when jogging. Higher values means more "bobbing".
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jog")
		float _fJogCameraBobScale = 0.4f;

	/*
	*	
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jog")
		float _fJogGroundFriction = 8.0f;

	/*
	*	Reference to the camera shake class to be used when the character is jogging.
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jog")
		TSubclassOf<class UCameraShake> _CameraShakeJog = NULL;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | Jump 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jump")
		bool _bCanJump = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Jump", Replicated)
		bool _bIsJumping = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jump")
		bool _bOverwriteJumpMovementComponent = false;
	
	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jump",
		meta = (EditCondition = "_bOverwriteJumpMovementComponent"))
		float _fOverwriteJumpForce = 740.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jump")
		TSubclassOf<class UCameraShake> _CameraShakeJumpStart = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jump")
		TSubclassOf<class UCameraShake> _CameraShakeJumpLand = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jump")
		TSubclassOf<class UCurveVector> _CurveJumpLand = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jump")
		float _fJumpGamepadRumbleIntensity = 1.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jump")
		float _fJumpGamepadRumbleDuration = 0.15f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jump")
		bool _fJumpGamepadRumbleAffectsLeftLarge = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jump")
		bool _fJumpGamepadRumbleAffectsLeftSmall = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jump")
		bool _fJumpGamepadRumbleAffectsRightLarge = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jump")
		bool _fJumpGamepadRumbleAffectsRightSmall = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Jump")
		float _fFallingLandShakeMultiplier = 2.75f;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | Slide 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Slide")
		bool _bOverrideSlideVelocityFromDash = true;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | Speed 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Speed")
		float _fMovementSpeedWalk = 575.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Speed")
		float _fMovementSpeedJog = 700.0f;
	
	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Speed")
		float _fMovementSpeedSprint = 835.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Speed")
		float _fMovementSpeedCrouch = 385.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Speed")
		float _fMovementAccelerationRate = 100.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Speed")
		bool _bUsesStaminaToSprint = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Speed")
		TSubclassOf<UStamina> _SprintStaminaComponent;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | Sprint 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*	Returns whether the character can use the sprint mechanic.
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Sprint")
		bool _bSprintEnabled = false;

	/*
	*	Returns whether the character is trying to sprint (usually given by player input)
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Sprint")
		bool _bTryingToSprint = false;

	/*
	*	Returns whether the character can sprint.
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Sprint")
		bool _bCanSprint = false;

	/*
	*	Returns whether the character is currently in a sprinting state or not (REPLICATED).
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Sprint", Replicated)
		bool _bIsSprinting = false;

	/*
	*	The amount of time the character has been sprinting for.
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Sprint")
		float _fSprintTime = 0.0f;

	/*
	*	The multiplier used against the player's look input when the character is strafing.
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Sprint")
		float _fSprintStrafeInputScale = 0.35f;

	/*
	*	The multiplier used against the player's look input when the character is sprinting.
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Sprint")
		float _fSprintLookInputScale = 0.7f;

	/*
	*	A scalar multiplier of the camera shake used when sprinting. Higher values means more "bobbing".
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Sprint")
		float _fSprintCameraBobScale = 2.0f;

	/*
	*	Reference to the camera shake class to be used when the character is sprinting.
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Sprint")
		TSubclassOf<class UCameraShake> _CameraShakeSprint = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintAssignable, Category = "Movement: Sprint")
		FOnSprintEnter _fOnSprintEnter;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Sprint")
		FTransform _tSprintWeaponOrigin;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | Stamina 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Inventory")
		TArray<UStamina*> _uStaminaComponents;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | Vault 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Vault")
		float _fLedgeForwardTraceRadius = 40.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Vault")
		float _fLedgeForwardTraceLength = 200.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Vault")
		float _fLedgeHeightTraceVerticalOffset= 200.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Vault")
		float _fLedgeHeightTraceForwardOffset = 100.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Vault")
		float _fLedgeHeightTraceLength = 300.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Vault")
		float _fVaultTime = 0.35;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Vault")
		FName _sPelvisSocket = TEXT("pelvisSocket");

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Vault")
		float _fLedgeGrabThresholdMin = -75.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Vault")
		float _fLedgeGrabThresholdMax = 50.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Vault")
		float _fVaultHeightOffset = 100.0f;
	
	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Vault")
		float _fVaultForwardOffset = 100.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Movement | Vault")
		TEnumAsByte<ETraceTypeQuery> _eVaultTraceChannel = ETraceTypeQuery::TraceTypeQuery15;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Vault")
		bool _bCanVault = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Vault", Replicated)
		bool _bIsVaulting = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Vault")
		bool _bIsTryingToVault = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Vault")
		FVector _vWallImpactPoint = FVector::ZeroVector;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Vault")
		FVector _vWallNormal = FVector::ZeroVector;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Vault")
		FVector _vWallTraceStart = FVector::ZeroVector;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Vault")
		FVector _vWallTraceEnd = FVector::ZeroVector;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Vault")
		FVector _vWallHeightLocation = FVector::ZeroVector;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Movement | Vault")
		int32 _NextUUID = 0;

#pragma endregion Protected Variables

#pragma region Public Functions

public:

	/**
	* @summary:	Sets default values for this component's properties.
	*
	* @return:	Constructor
	*/
	ABaseCharacter();

	/**
	* @summary:	Called when the game starts or when spawned.
	*
	* @return:	virtual void
	*/
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Character | FirstPerson 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure) USkeletalMeshComponent* GetFirstPersonArmsDuelLeftMesh() { return _FirstPerson_ArmsDuelLeft; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) USkeletalMeshComponent* GetFirstPersonArmsDuelRightMesh() { return _FirstPerson_ArmsDuelRight; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) USkeletalMeshComponent* GetFirstPersonArmsMesh() { return _FirstPerson_Arms; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) USkeletalMeshComponent* GetFirstPersonPrimaryWeaponMesh() { return _FirstPerson_PrimaryWeapon_SkeletalMesh; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) USkeletalMeshComponent* GetFirstPersonSecondaryWeaponMesh() { return _FirstPerson_SecondaryWeapon_SkeletalMesh; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) USkeletalMeshComponent* GetThirdPersonPrimaryWeaponMesh() { return _ThirdPerson_PrimaryWeapon_SkeletalMesh; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) USkeletalMeshComponent* GetThirdPersonSecondaryWeaponMesh() { return _ThirdPerson_SecondaryWeapon_SkeletalMesh; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Animation 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void FreezeAnimation(USkeletalMeshComponent* ArmsMesh, UAnimMontage* MontageToFreeze, float EndFrame, bool bHideMeshOnFreeze);

	/*
	*
	*/
	UFUNCTION(BlueprintPure) float GetGlobalReloadPlayRate() const { return _fGlobalReloadPlayRate; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Current Frame 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @summary:	Called every frame.
	*
	* @param:	float DeltaTime
	*
	* @return:	virtual void
	*/
	virtual void Tick(float DeltaTime) override;

	/*
	*
	*/
	virtual void OnGroundChecks(float DeltaTime);

	/*
	*
	*/
	void TickAiming(float DeltaTime);

	/*
	*
	*/
	void TickFiringPrimary(float DeltaTime);

	/*
	*
	*/
	void TickFiringSecondary(float DeltaTime);

	/*
	*
	*/
	void TickReloadingPrimary(float DeltaTime);

	/*
	*
	*/
	void TickReloadingSecondary(float DeltaTime);

	/*
	*
	*/
	void TickSprint(float DeltaTime);

	/*
	*
	*/
	void TickCrouch(float DeltaTime);

	/*
	*
	*/
	void TickShields(float DeltaTime);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Camera 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Client, Unreliable, WithValidation)
		void OwningClient_PlayCameraShake(TSubclassOf<class UCameraShake> ShakeClass, float Scale);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Controller | Gamepad 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Client, Unreliable, WithValidation)
		void OwningClient_GamepadRumble(float Intensity, float Duration,
										bool AffectsLeftLarge, bool AffectsLeftSmall, bool AffectsRightLarge, bool AffectsRightSmall);

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
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SetUseControllerRotationYaw(bool useControllerRotationYaw);

	UFUNCTION()
		void SetForwardInputScale(float ForwardInputScale);

	/**
	* @summary:	Sets the whether forward input scale of the character (used for animation)
	*
	* @networking:	Runs on server
	*
	* @param:	float forwardInputScale
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SetForwardInputScale(float ForwardInputScale);

	UFUNCTION()
		void SetRightInputScale(float RightInputScale);

	/**
	* @summary:	Sets the whether right input scale of the character (used for animation)
	*
	* @networking:	Runs on server
	*
	* @param:	float rightInputScale
	*
	* @return:	void
	*/
	UFUNCTION(Server, Unreliable, WithValidation)
		void Server_SetRightInputScale(float RightInputScale);

	/**
	* @summary:	Returns the current forward input scale (used for animation walk blending)
	*
	* @return:	float
	*/
	UFUNCTION(BlueprintPure, Category = "Controller | Input")
		float GetForwardInputScale() { return _fForwardInputScale; }

	/**
	* @summary:	Returns the current right input scale (used for animation walk blending)
	*
	* @return:	float
	*/
	UFUNCTION(BlueprintPure, Category = "Controller | Input")
		float GetRightInputScale() { return _fRightInputScale; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Health | 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		virtual void OnAnyDamage(AActor* Actor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_OnAnyDamage(AActor* Actor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	/*
	*
	*/
	UFUNCTION()
		virtual void OnPointDamage(float Damage);

	/*
	*
	*/
	UFUNCTION()
		virtual void OnRadialDamage(float Damage);

	/*
	*
	*/
	UFUNCTION()
		virtual void OnDeath();

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_OnDeath();

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		bool IsAlive() { return /*_bIsAlive = */_fFleshHealth > 0.0f; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Health | Burn 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_BurnCharacter(int Steps, float Damage, float Delay);

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
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_ResetFleshHealth();

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		float GetCurrentFleshHealth() { return _fFleshHealth; }

	// Returns the value of _MAX_FLESH_HEALTH
	UFUNCTION(BlueprintPure)
		float GetMaxFleshHealth() { return (float)_MAX_FLESH_HEALTH; }

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
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_ResetShield();

	/*
	*
	*/
	UFUNCTION()
		void ResetShieldRecharge();

	/*
	*
	*/
	UFUNCTION()
		void StartRechargingShields();

	/*
	*	Returns if _fShield <= 0
	*/
	UFUNCTION(BlueprintPure) bool IsShieldDepleted() { return _fShield <= 0.0f; }

	/*
	*	Returns the value of _fShield
	*/
	UFUNCTION(BlueprintPure) float GetCurrentShield() { return _fShield; }

	/*
	*	Returns the value of _MAX_SHIELD
	*/
	UFUNCTION(BlueprintPure) float GetMaxShield() { return (float)_MAX_SHIELD; }

	/*
	*	Returns the value of _fShieldRechargeDelay
	*/
	UFUNCTION(BlueprintPure) float GetShieldRechargeDelayTime() { return _fShieldRechargeDelay; }
	
	/*
	*	Returns the value of _fShieldRechargeDelayHandle
	*/
	UFUNCTION(BlueprintPure) FTimerHandle GetShieldRechargeDelayTimerHandle() { return _fShieldRechargeDelayHandle;}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Interaction

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Category = "Interaction")
		UInteractionDataComponent* CalculatePrimaryFocusInteractable();

	/*
	*
	*/
	UFUNCTION(Category = "Interaction")
		void AddToInteractablesArray(UInteractionDataComponent* Interactable);

	/*
	*
	*/
	UFUNCTION(Category = "Interaction")
		bool RemoveFromInteractablesArray(UInteractionDataComponent* Interactable);

	/*
	*
	*/
	UFUNCTION(Category = "Interaction")
		void InteractPrimary();

	/*
	*
	*/
	UFUNCTION(Category = "Interaction")
		void InteractSecondary();

	/*

	*/
	UFUNCTION(Category = "Interaction")
		void CancelInteraction();

	/*

	*/
	UFUNCTION(Category = "Interaction")
		void Interact(bool IsSecondary);

	/*

	*/
	UFUNCTION()
		void FocusInteractableToHUD();

	/*

	*/
	UFUNCTION()
		void SetIsTryingToInteractPrimary(bool Interacting) { _bIsTryingToInteractPrimary = Interacting; }

	/*

	*/
	UFUNCTION()
		void SetIsTryingToInteractSecondary(bool Interacting) { _bIsTryingToInteractSecondary = Interacting; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Interaction")
		TArray<UInteractionDataComponent*> GetInteractablesArray() { return _Interactables; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Interaction")
		FTimerHandle GetInteractionHandle() const { return _fInteractionHandle; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Starting

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @summary:	Spawns the starting loadout weapons and assigns them to the character.
	*
	* @networking:	Runs on server
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory | Starting")
		void Server_Reliable_CreateStartingLoadout();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Weapon | Aiming

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @summary:	Sets the whether the character is aiming or not
	*
	* @networking:	Runs on server
	*
	* @param:	bool aiming
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetIsAiming(bool aiming);

	/*
	*
	*/
	UFUNCTION()
		virtual void InputAimEnter();

	/*
	*
	*/
	UFUNCTION()
		virtual void InputAimExit();

	/*
	*
	*/
	UFUNCTION()
		void StopAiming();

	/**
	* @summary:	Returns whether the character is aiming or not.
	*
	* @return:	bool
	*/
	UFUNCTION(BlueprintPure) bool IsAiming() { return _bIsAiming; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Weapon | Duel Wielding

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void OnDuelWielding_PrimaryReloadComplete();

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetIsDuelWielding(bool bDuelWielding);

	/*
	*
	*/
	UFUNCTION()
		void SetIsDuelWielding(bool bDuelWielding);

	/*
	*
	*/
	UFUNCTION()
		virtual void OnRep_DuelWielding();

	/*
	*
	*/
	UFUNCTION(BlueprintPure) bool IsDuelWielding() const { return _bIsDuelWielding; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) bool IsDuelWieldingMirrored() const { return _bDuelWieldingIsMirrored; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Weapon | Firing

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void FireWeaponTraceFullAuto(AWeapon* Weapon);

	/*
	*
	*/
	UFUNCTION()
		void FireWeaponTraceSemiAuto(AWeapon* Weapon);

	/*
	*
	*/
	UFUNCTION()
		void FireWeaponTraceBurst(AWeapon* Weapon);

	/*
	*
	*/
	UFUNCTION()
		void FireWeaponTraceSpread(AWeapon* Weapon);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Weapon  | Toggle

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void InputToggleWeapon();

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetupToggleWeapon();

	/*
	*	Setup toggle environment (old primary/reserve pointer refs) && notify state change
	*/
	UFUNCTION()
		void SetupToggleWeapon();

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_ToggleWeapon();

	/*
	*
	*/
	UFUNCTION()
		void ToggleWeapon();

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_ExitToggleWeapon();

	/*
	*
	*/
	UFUNCTION()
		void ExitToggleWeapon();

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_DropInteractableWeapon(AWeapon* WeaponInstance);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SpawnAbstractWeapon(bool IsSecondary, TSubclassOf<AWeapon> WeaponClass, int MagazineSize, int ReserveSize, int BatterySize);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Weapon | Primary

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		virtual void OnRep_PrimaryWeapon();

	/**
	* @summary:	Sets the character's primary weapon
	*
	* @networking:	Runs on server
	*
	* @param:	AWeapon*	Weapon
	* @param:	bool		FirstPickup
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetPrimaryWeapon(AWeapon* Weapon, bool PlayAnimation, bool FirstPickup, bool DestroyOld = true);

	/**
	* @summary:	Sets the character's first person mesh based on their primary weapon
	*
	* @networking:	Runs on owning client
	*
	* @param:	AWeapon*	Weapon
	*
	* @return:	void
	*/
	UFUNCTION(Client, Unreliable, WithValidation)
		void OwningClient_UpdateFirstPersonPrimaryWeaponMesh(AWeapon* Weapon, bool PlayAnimation, bool FirstPickup);

	/**
	* @summary:	Sets the character's third person mesh based on their primary weapon
	*
	* @networking:	Runs on all clients
	*
	* @param:	AWeapon*	Weapon
	*
	* @return:	void
	*/
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
		void Multicast_UpdateThirdPersonPrimaryWeaponMesh(AWeapon* Weapon);

	/*
	*
	*/
	UFUNCTION(BlueprintCosmetic, Category = "Inventory | Weapon | Primary")
		void UpdateFirstPersonPrimaryScopeAttachment(AWeapon* Weapon);

	/*
	*
	*/
	UFUNCTION(Client, Reliable, WithValidation)
		void OwningClient_PlayPrimaryWeaponFPAnimation(USkeletalMeshComponent* ArmsMesh,
			float PlayRate, bool FreezeMontageAtLastFrame,
			bool PlayHandAnimation, uint8 HandAnimation, float HandAnimationStartingFrame,
			bool PlayGunAnimation, uint8 GunAnimation, float GunAnimationStartingFrame);

	/*
	*
	*/
	UFUNCTION()
		void PlayPrimaryWeaponFPAnimation(USkeletalMeshComponent* ArmsMesh,
			float PlayRate, bool FreezeMontageAtLastFrame,
			bool PlayHandAnimation, uint8 HandAnimation, float HandAnimationStartingFrame,
			bool PlayGunAnimation, uint8 GunAnimation, float GunAnimationStartingFrame);

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		AWeapon* GetPointerPrimaryWeapon() const { return _PrimaryWeapon; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Inventory | Weapon | Primary | Firing
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	virtual void InitFirePrimaryWeapon();

	/*
	*
	*/
	UFUNCTION()
		void InputPrimaryFirePress();

	/*
	*
	*/
	UFUNCTION()
		void InputPrimaryFireRelease();

	/*
	*
	*/
	UFUNCTION(Client, Reliable, WithValidation)
		void OwningClient_Reliable_PrimarySetCanFireWeapon(bool bCanFire);

	/*
	*
	*/
	UFUNCTION(Client, Reliable, WithValidation)
		void OwningClient_Reliable_PrimaryWeaponCameraTrace();

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_PrimaryWeaponCameraTrace(FHitResult ClientHitResult);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Weapon | Primary | Reload

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @summary:	Checks and initiates a reload of the character's primary weapon.
	*
	* @return:	virtual void
	*/
	UFUNCTION()
		virtual void InputReloadPrimaryWeapon();

	/*
	*
	*/
	UFUNCTION()
		void OnReloadComplete();

	/*
	*
	*/
	UFUNCTION()
		void SetIsReloadingPrimaryWeapon(bool IsReloading);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetIsReloadingPrimaryWeapon(bool ReloadingPrimary);

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		bool IsPrimaryReloadCanceled() const { return _bPrimaryReloadCancelled; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Weapon | Secondary

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		virtual void OnRep_SecondaryWeapon();

	/**
	* @summary:	Sets the character's secondary weapon
	*
	* @networking:	Runs on server
	*
	* @param:	AWeapon* Weapon
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory | Weapon | Secondary")
		void Server_Reliable_SetSecondaryWeapon(AWeapon* Weapon);

	/**
	* @summary:	Sets the character's first person mesh based on their secondary weapon
	*
	* @networking:	Runs on owning client
	*
	* @param:	AWeapon*	Weapon
	*
	* @return:	void
	*/
	UFUNCTION(Client, Unreliable, WithValidation, Category = "Inventory | Weapon | Secondary")
		void OwningClient_UpdateFirstPersonSecondaryWeaponMesh(AWeapon* Weapon);

	/**
	* @summary:	Sets the character's third person mesh based on their secondary weapon
	*
	* @networking:	Runs on all clients
	*
	* @param:	AWeapon*	Weapon
	*
	* @return:	void
	*/
	UFUNCTION(NetMulticast, Unreliable, WithValidation, Category = "Inventory | Weapon | Secondary")
		void Multicast_UpdateThirdPersonSecondaryWeaponMesh(AWeapon* Weapon);

	/*
	*
	*/
	UFUNCTION(Client, Reliable, WithValidation)
		void OwningClient_PlaySecondaryWeaponFPAnimation(USkeletalMeshComponent* ArmsMesh,
			float PlayRate, bool FreezeMontageAtLastFrame,
			bool PlayHandAnimation, uint8 HandAnimation, float HandAnimationStartingFrame,
			bool PlayGunAnimation, uint8 GunAnimation, float GunAnimationStartingFrame);

	/*
	*
	*/
	UFUNCTION()
		void PlaySecondaryWeaponFPAnimation(USkeletalMeshComponent* ArmsMesh,
			float PlayRate, bool FreezeMontageAtLastFrame,
			bool PlayHandAnimation, uint8 HandAnimation, float HandAnimationStartingFrame,
			bool PlayGunAnimation, uint8 GunAnimation, float GunAnimationStartingFrame);

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		AWeapon* GetPointerSecondaryWeapon() const { return _SecondaryWeapon; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Weapon | Secondary | Firing

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void InitFireSecondaryWeapon();

	/*
	*
	*/
	UFUNCTION()
		void InputSecondaryFirePress();

	/*
	*
	*/
	UFUNCTION()
		void InputSecondaryFireRelease();

	/*
	*
	*/
	UFUNCTION(Client, Reliable, WithValidation, Category = "Inventory | Weapon | Primary")
		void OwningClient_Reliable_SecondaryWeaponCameraTrace();

	UFUNCTION(Server, Reliable, WithValidation, Category = "Inventory | Weapon | Primary")
		void Server_Reliable_SecondaryWeaponCameraTrace(FHitResult ClientHitResult);

	/*
	*
	*/
	UFUNCTION(Client, Reliable, WithValidation)
		void OwningClient_Reliable_SecondarySetCanFireWeapon(bool bCanFire);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Weapon | Secondary | Reload

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @summary:	Checks and initiates a reload of the character's secondary weapon.
	*
	* @return:	virtual void
	*/
	UFUNCTION()
		virtual void InputReloadSecondaryWeapon();

	/*
	*
	*/
	UFUNCTION()
		void OnDuelWielding_SecondaryReloadComplete();

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetIsReloadingSecondaryWeapon(bool ReloadingSecondary);

	/*
	*
	*/
	UFUNCTION()
		void SetIsReloadingSecondaryWeapon(bool IsReloading);

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		bool IsSecondaryReloadCanceled() const { return _bSecondaryReloadCancelled; }

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
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetReserveWeapon(AWeapon* Weapon);

	/*
	*
	*/
	UFUNCTION()
		virtual void OnRep_ReserveWeapon();

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		AWeapon* GetPointerReserveWeapon() const { return _ReserveWeapon; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Inventory | Grenade

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetFragmentationGrenadeCount() const { return _iFragmentationGrenadeCount; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetEMPGrenadeCount() const { return _iEMPGrenadeCount; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetIncendiaryGrenadeCount() const { return _iIncendiaryGrenadeCount; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		E_GrenadeType GetCurrentGrenadeType() const { return _eCurrentGrenadeType; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | Base

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		virtual void MoveForward(float Value);

	/*
	*
	*/
	UFUNCTION()
		virtual void MoveRight(float Value);

	UFUNCTION(Server, Unreliable, WithValidation)
		void Server_SetMovementMode(EMovementMode MovementMode);

	UFUNCTION(NetMulticast, Unreliable, WithValidation)
		void Multicast_SetMovementMode(EMovementMode MovementMode);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | Properties | Gravity

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | Properties | Speed

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Movement | Properties | Speed")
		void SetMovingSpeed(float Speed);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetMovingSpeed(float Speed);

	/*
	*
	*/
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_Reliable_SetMovingSpeed(float Speed);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | Properties | Stamina 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @summary:	Returns reference to the list of stamina components attached to this character
	*
	* @return:	TArray<UStamina*>
	*/
	UFUNCTION(BlueprintPure, Category = "Inventory")
		TArray<UStamina*> GetStaminaComponents() { return _uStaminaComponents; }

	/**
	* @summary:	Returns reference to the of stamina component attached to this character, specified by the channel.
	*
	* @param:	int Channel
	*
	* @return:	TArray<UStamina*>
	*/
	UFUNCTION(BlueprintPure, Category = "Inventory")
		UStamina* GetStaminaComponentByChannel(int Channel);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | States | Crouch 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		virtual void InputCrouchToggle(bool Crouch);

	/*
	*
	*/
	UFUNCTION()
		void InputCrouchEnter();

	/*
	*
	*/
	UFUNCTION()
		void InputCrouchExit();

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetIsCrouching(bool IsCrouching);

	/**
	* @summary:	Returns whether the character is crouching or not.
	*
	* @return:	bool
	*/
	UFUNCTION(BlueprintPure, Category = "Movement | Crouch")
		bool IsCrouching() { return _bIsCrouching; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | States | Jump

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		virtual void InputJump();

	/**
	* @summary:	Sets the whether the character is jumping or not.
	*
	* @networking:	Runs on server
	*
	* @param:	bool Jumping
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetJumping(bool Jumping);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | States | Slide

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		virtual void InputSlideEnter();

	/*
	*
	*/
	UFUNCTION()
		virtual void InputSlideExit();

	/*
	*
	*/
	UFUNCTION()
		virtual void InputSlideToggle(bool Sliding);

	/*
	*
	*/
	UFUNCTION()
		virtual void Slide(bool WasDashing = false);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetIsSliding(bool Sliding);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_InitiateSlide(bool WasDashing);

	/*
	*
	*/
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_Reliable_InitiateSlide(bool WasDashing);

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

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Movement | Slide") bool IsSliding() const { return _bIsSliding; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | States | Sprint

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @summary:	Sets the whether the character is using sprinting or not.
	*
	* @networking:	Runs on server
	*
	* @param:	bool Sprinting
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetSprinting(bool Sprinting);

	/**
	* @summary:	Returns whether the character is sprinting or not.
	*
	* @return:	bool
	*/
	UFUNCTION(BlueprintPure, Category = "Movement | Sprint")
		bool IsSprinting() { return _bIsSprinting; }

	/**
	* @summary:	Returns the look input scalar when the character is sprinting.
	*
	* @return:	float
	*/
	UFUNCTION()
		float GetSprintLookInputScale() { return _fSprintLookInputScale; }

	/*
	*
	*/
	UFUNCTION()
		void InputSprintEnter();

	/*
	*
	*/
	UFUNCTION()
		void InputSprintExit();

	/*
	*
	*/
	UFUNCTION()
		void InputSprintToggle(bool Sprint);

	/*
	*
	*/
	UFUNCTION()
		void StopSprinting();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Movement | States | Vault 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void InputVault();

	/*
	*
	*/
	UFUNCTION()
		FHitResult LedgeForwardTrace();

	/*
	*
	*/
	UFUNCTION()
		FHitResult LedgeHeightTrace();

	/*
	*
	*/
	UFUNCTION()
		bool GetHipToLedge();

	/*
	*
	*/
	UFUNCTION()
		FVector GetMoveToLocation(float HeightOffset, float ForwardOffset);

	/*
	*
	*/
	UFUNCTION()
		void GrabLedge(FVector MoveLocation);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_GrabLedge(FVector MoveLocation);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_Reliable_GrabLedge(FVector MoveLocation);

	/*
	*
	*/
	UFUNCTION()
		void ClimbLedge();

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetIsVaulting(bool Vaulting);
	
	/*
	*
	*/
	UFUNCTION()
		bool IsTryingToVault() { return _bIsTryingToVault; }

	/*
	*	Doing ++ after the variable means add one to it but returns the value before the addition, 
	*	where as if we did it before it would return the value after the addition.
	*/
	UFUNCTION()
		int32 GetNextUUID() { return _NextUUID++; }

	// TEMPORARILY PLACED HERE FOR EASE OF USE -> TO BE MOVED INTO A SEPARATE CLASS LATER
	const FString EnumToString(const TCHAR* Enum, int32 EnumValue)
	{
		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, Enum, true);
		if (!EnumPtr)
			return NSLOCTEXT("Invalid", "Invalid", "Invalid").ToString();

#if WITH_EDITOR
		return EnumPtr->GetDisplayNameTextByIndex(EnumValue).ToString();
#else
		return EnumPtr->GetEnumName(EnumValue);
#endif
	}

#pragma endregion Public Functions

};
