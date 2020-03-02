// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon.h"
#include "FireMode.generated.h"

// *** DEFINITIONS

#define _MAX_MAGAZINE_AMMO	100
#define _MAX_RESERVE_AMMO	100
#define _MAX_DAMAGE			100

// *** ENUMERATORS

UENUM(BlueprintType)
enum class E_FiringModeType : uint8
{
	eFMT_SemiAuto UMETA(DisplayName = "Semi Automatic"),
	eFMT_FullAuto UMETA(DisplayName = "Fully Automatic"),
	eFMT_Burst UMETA(DisplayName = "Controlled Burst"),
	eFMT_Spread UMETA(DisplayName = "Spread")
};

UENUM(BlueprintType)
enum class E_FiringTriggerType : uint8
{
	eFTT_Press UMETA(DisplayName = "LMB_Press"),
	eFTT_Release UMETA(DisplayName = "LMB_Release")
};

UENUM(BlueprintType)
enum class E_ProjectileType : uint8
{
	ePT_Raycast UMETA(DisplayName = "Hitscan"),
	ePT_Physics UMETA(DisplayName = "Physical")
};

UENUM(BlueprintType)
enum class E_DelayDirectionType : uint8
{
	eDDT_Additive UMETA(DisplayName = "Additive"),
	eDDT_Subtractive UMETA(DisplayName = "Subtractive")
};

UENUM(BlueprintType)
enum class E_ModeActivationType : uint8
{
	eMAT_ZoomEnter UMETA(DisplayName = "Zooming In"),
	eMAT_ZoomExit UMETA(DisplayName = "Zooming Out"),
	eMAT_Toggle UMETA(DisplayName = "Manual Toggle"),
	eMAT_Charged UMETA(DisplayName = "Fully Charged")
};

UENUM(BlueprintType)
enum class E_ReloadType : uint8
{
	eRT_Magazine UMETA(DisplayName = "Magazine"),
	eRT_LoadSingle UMETA(DisplayName = "Load Each Round")
};

UENUM(BlueprintType)
enum class E_ReloadStage : uint8
{
	eRS_Ready UMETA(DisplayName = "Ready"),
	eRS_EjectMagazine UMETA(DisplayName = "Eject Magazine"),
	eRS_NewMagazine UMETA(DisplayName = "New Magazine"),
	eRS_ChamberRound UMETA(DisplayName = "Chamber Round")
};

// *** CLASSES

class UAmmo;
class UCrosshair;
class UTimelineComponent;
class UUserWidget;
class AProjectile;

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ARENASHOOTER_API UFireMode : public UActorComponent
{
	GENERATED_BODY()

public:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Startup ********************************************************************************************************************************

	/**
	* @summary:	Sets default values for this component's properties.
	*
	* @return:	Constructor
	*/
	UFireMode();

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

	///////////////////////////////////////////////

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_CreateImpactManager();

	// Firing *********************************************************************************************************************************
	
	/*
	*
	*/
	void FireProjectile(FHitResult hitResult, FVector CameraRotationXVector, USkeletalMeshComponent* SkCharWepMeshFirstP, USkeletalMeshComponent* SkCharWepMeshThirdP);

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// Current Frame **************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		float _fProjectileSpread = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		float _fWeaponHeat = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		float _fFiringDelay = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		int _iBatteryCapacity = 0;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		int _iMagazineAmmoCount = 0;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		int _iReserveAmmoCount = 0;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		bool _bHitscanPassedMaxRange = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		float _fHitscanDamageToPass = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		bool _bIsFullyCharged = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		bool _bIsOverheated = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		bool _bIsMisfired = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		float _fRecoilInterpolationSpeed = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		float _fFiringTime = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		bool _bCanTryToVentCooldown = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		float _fHeatDecreaseMultiplier = 1.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		bool _bIsMagazineInWeapon = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		bool _bIsRoundInChamber = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		bool _bHasBeenChambered = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		bool _bIsFiring = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		AWeapon* _WeaponParentAttached = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		bool _bFireDelayComplete = true;

	/*
	*	A timer handle used for referencing the firing delay.
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		FTimerHandle _fFireDelayHandle;

	/*
	*	A timer handle used for referencing the projectile spread.
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		FTimerHandle _fProjectileSpreadHandle;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		bool _bCanDeductSpread = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		bool _bReloadComplete = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		E_ReloadStage _eReloadStage = E_ReloadStage::eRS_Ready;

	/*
	*	A timer handle used for referencing the reload stage.
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		FTimerHandle _fReloadStageHandle;

	// Data Table *****************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data Table")
		bool _bUseDataTable = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Data Table",
		meta = (EditCondition = "_bUseDataTable"))
		FString _sDataTableLocation;

	// Activation *****************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Activation")
		E_ModeActivationType _FireModeActivationType = E_ModeActivationType::eMAT_Toggle;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Activation")
		int _iFiringModeIterator = NULL;

	// Aim Assist *****************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aim Assist",
		meta = (EditCondition = "!_bUseDataTable"))
		float _BulletMagnetismTraceRadius = 0.0f;

	// Aiming *********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Aiming")
		bool _bAimDownSightEnabled = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Aiming")
		float _fAimTime = 1.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Aiming")
		TArray<float> _fFovAiming;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Aiming",
		meta = (EditCondition = "_bAimDownSightEnabled"))
		FTransform _tAdsOriginHands;

	// Ammo ***********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo")
		int _iAmmoChannel = 0;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Ammo")
		UAmmo* _pAmmoPool = NULL;

	// Animation ******************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		FAnimationListArms _AnimationMontageListHands;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		FAnimationListWeapon _AnimationMontageListWeapon;

	// Burn **********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Burn",
		meta = (EditCondition = "!_bUseDataTable"))
		bool _bBurnsTargetOnImpact = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Burn")
		bool _bBurnUsesDataTable = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Burn",
		meta = (EditCondition = "_bBurnUsesDataTable"))
		FString _sBurnDataTableLocation;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Burn",
		meta = (EditCondition = "!_bBurnUsesDataTable"))
		int _iBurnIntervals = 3;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Burn",
		meta = (EditCondition = "!_bBurnUsesDataTable"))
		float _fBurnDamage = 5.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Burn",
		meta = (EditCondition = "!_bBurnUsesDataTable"))
		float _fBurnStepDelay = 0.5;

	// Burst **********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Burst",
		meta = (EditCondition = "!_bUseDataTable"))
		int _iBurstFireShotCount = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Burst",
		meta = (EditCondition = "!_bUseDataTable"))
		float _fBurstFireShotDelay = 0.0f;

	// Chambering *****************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Chambering",
		meta = (EditCondition = "!_bUseDataTable"))
		bool _bEachFireRequiresChambering = false;

	// Charging *******************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Charging",
		meta = (EditCondition = "!_bUseDataTable"))
		bool _bWeaponMustBeChargedToFire = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Charging")
		bool _bChargingUsesDataTable = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Charging",
		meta = (EditCondition = "_bChargingUsesDataTables"))
		FString _sChargingDataTableLocation;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Charging",
		meta = (EditCondition = "!_bChargingUsesDataTables && _bWeaponMustBeChargedToFire"))
		float _fChargingTime = 1.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Charging",
		meta = (EditCondition = "!_bChargingUsesDataTables && _bWeaponMustBeChargedToFire"))
		int _iShotsFiredPerCharge = 1.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Charging",
		meta = (EditCondition = "!_bChargingUsesDataTables && _bWeaponMustBeChargedToFire"))
		bool _bAutomaticallyFireOnCharge = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Charging",
		meta = (EditCondition = "!_bChargingUsesDataTables && _bWeaponMustBeChargedToFire"))
		bool _bDisappatesOnFullCharge = false;

	// Contrail *******************************************************************************************************************************

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Contrail")
		UParticleSystem* _pContrailParticleSystem = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Contrail")
		FName _fContrailName;

	// Damage *********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Damage")
		bool _bDamageUsesDataTable = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Damage",
		meta = (EditCondition = "_bDamageUsesDataTable"))
		FString _sDamageDataTableLocation;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Damage",
		meta = (EditCondition = "!_bDamageUsesDataTable"))
		float _fMaxRangeThreshold = 10000.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Damage",
		meta = (EditCondition = "!_bDamageUsesDataTable"))
		bool _bLosesDamageOverDistance = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Damage",
		meta = (EditCondition = "!_bDamageUsesDataTable && _bLosesDamageOverDistance"))
		UCurveFloat* _cDamageDistanceCurve = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Damage",
		meta = (EditCondition = "!_bDamageUsesDataTable"))
		bool _bInstaKillOnHeadshot = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Damage",
		meta = (EditCondition = "!_bDamageUsesDataTable && !_bInstaKillOnHeadshot"))
		float _fDamageHead = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Damage",
		meta = (EditCondition = "!_bDamageUsesDataTable"))
		float _fDamageTorso = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Damage",
		meta = (EditCondition = "!_bDamageUsesDataTable"))
		float _fDamageArms = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Damage",
		meta = (EditCondition = "!_bDamageUsesDataTable"))
		float _fDamageLegs = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Damage",
		meta = (EditCondition = "!_bDamageUsesDataTable"))
		float _fDamageShield = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Damage",
		meta = (EditCondition = "!_bDamageUsesDataTable"))
		float _fDamageShieldHead = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Damage",
		meta = (EditCondition = "!_bDamageUsesDataTable"))
		float _fDamageBase = 0.0f;

	// Delay **********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Delay")
		bool _bDelayUsesDataTable = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Delay",
		meta = (EditCondition = "_bChargingUsesDataTables"))
		FString _sDelayDataTableLocation;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Delay",
		meta = (EditCondition = "!_bDelayUsesDataTable"))
		E_DelayDirectionType _eDelayDirection = E_DelayDirectionType::eDDT_Additive;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Delay",
		meta = (EditCondition = "!_bDelayUsesDataTable"))
		float _fFiringDelayMinimum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Delay",
		meta = (EditCondition = "!_bDelayUsesDataTable"))
		float _fFiringDelayMaximum = 2.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Delay",
		meta = (EditCondition = "!_bDelayUsesDataTable"))
		float _fDelayDepreciation = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Delay",
		meta = (EditCondition = "!_bDelayUsesDataTable"))
		float _fFiringDelayResetRate = 1.0f;

	// Firing *********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		int _iFiringModeIdentifier = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		E_FiringModeType _FiringType;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		E_FiringTriggerType _FiringTrigger;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		float _fGamepadRumbleFiringIntensity = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		float _fGamepadRumbleFiringDuration = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		bool _bGamepadRumbleFiringAffectsLeftLarge_Primary = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		bool _bGamepadRumbleFiringAffectsLeftSmall_Primary = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		bool _bGamepadRumbleFiringAffectsRightLarge_Primary = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		bool _bGamepadRumbleFiringAffectsRightSmall_Primary = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		bool _bGamepadRumbleFiringAffectsLeftLarge_DuelPrimary = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		bool _bGamepadRumbleFiringAffectsLeftSmall_DuelPrimary = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		bool _bGamepadRumbleFiringAffectsRightLarge_DuelPrimary = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		bool _bGamepadRumbleFiringAffectsRightSmall_DuelPrimary = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		bool _bGamepadRumbleFiringAffectsLeftLarge_DuelSecondary = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		bool _bGamepadRumbleFiringAffectsLeftSmall_DuelSecondary = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		bool _bGamepadRumbleFiringAffectsRightLarge_DuelSecondary = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Firing")
		bool _bGamepadRumbleFiringAffectsRightSmall_DuelSecondary = false;

	// Heat ***********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Heat")
		bool _bCanOverheat = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Heat")
		bool _bOverheatUsesDataTable = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Heat",
		meta = (EditCondition = "_bOverheatUsesDataTable"))
		FString _sOverheatDataTableLocation;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Heat",
		meta = (EditCondition = "!_bOverheatUsesDataTable && _bCanOverheat"))
		float _fMaxHeat = 100.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Heat",
		meta = (EditCondition = "!_bOverheatUsesDataTable && _bCanOverheat"))
		float _fHeatIncreaseAmount = 5.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Heat",
		meta = (EditCondition = "!_bOverheatUsesDataTable && _bCanOverheat"))
		float _fHeatDecreaseAmount = 3.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Heat",
		meta = (EditCondition = "!_bOverheatUsesDataTable && _bCanOverheat"))
		bool _bCanVentHeat = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Heat",
		meta = (EditCondition = "!_bOverheatUsesDataTable && _bCanOverheat && _bCanVentHeat", 
			ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float _fCoolingVentRangeMaximum = 0.5f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Heat",
		meta = (EditCondition = "!_bOverheatUsesDataTable && _bCanOverheat && _bCanVentHeat", 
			ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float _fCoolingVentRangeMinimum = 0.4f;

	// Impact Effects *************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Impact Effects")
		TSubclassOf<class AImpactEffectManager> _ImpactEffectManagerClass = NULL;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Impact Effects", Replicated)
		AImpactEffectManager* _ImpactEffectManager = NULL;

	// Muzzle Effect **************************************************************************************************************************

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Muzzle Effect")
		UParticleSystem* _pMuzzleEffectParticleSystem = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Muzzle Effect")
		FName _MuzzleSocketName = TEXT("MuzzleLaunchPoint");

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Muzzle Effect")
		UParticleSystemComponent* _pLocalMuzzleEffect = NULL;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Muzzle Effect")
		UParticleSystemComponent* _pThirdPersonMuzzleEffect = NULL;

	// Projectile *****************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
		E_ProjectileType _eProjectileType = E_ProjectileType::ePT_Raycast;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<AProjectile> _uProjectileClass = NULL;

	// Projectile Spread **********************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile Spread")
		bool _bSpreadUsesDataTable = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile Spread",
		meta = (EditCondition = "_bSpreadUsesDataTable"))
		FString _sSpreadDataTableLocation;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile Spread",
		meta = (EditCondition = "!_bSpreadUsesDataTable"))
		float _fSpreadHipMinimum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile Spread",
		meta = (EditCondition = "!_bSpreadUsesDataTable"))
		float _fSpreadHipMaximum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile Spread",
		meta = (EditCondition = "!_bSpreadUsesDataTable"))
		float _fSpreadHipIncrease = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile Spread",
		meta = (EditCondition = "!_bSpreadUsesDataTable"))
		float _fSpreadHipDecrease = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile Spread",
		meta = (EditCondition = "!_bSpreadUsesDataTable"))
		float _fSpreadAimingMinimum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile Spread",
		meta = (EditCondition = "!_bSpreadUsesDataTable"))
		float _fSpreadAimingMaximum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile Spread",
		meta = (EditCondition = "!_bSpreadUsesDataTable"))
		float _fSpreadAimingIncrease = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile Spread",
		meta = (EditCondition = "!_bSpreadUsesDataTable"))
		float _fSpreadAimingDecrease = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile Spread",
		meta = (EditCondition = "!_bSpreadUsesDataTable"))
		float _fSpreadDecreaseInitialDelay = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile Spread",
		meta = (EditCondition = "!_bSpreadUsesDataTable"))
		float _fSpreadDecreaseMultiplierWhenFiringComplete = 0.0f;

	// Recoil *********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil")
		TSubclassOf<class UCameraShake> _HipfireRecoilCamShakeClass;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil")
		TSubclassOf<class UCameraShake> _AimingRecoilCamShakeClass;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil")
		bool _bRecoilUsesDataTable = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "_bRecoilUsesDataTable"))
		FString _sRecoilDataTableLocation;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		float _fCurrentRecoilInterpolationSpeed = 0.1f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		bool _bUpdateRecoilInterpolation = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable", ClampMin = 0.0f, ClampMax = 100.0f))
		float _fRecoilInterpolationSpeedHipfireMinimum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable", ClampMin = 0.0f, ClampMax = 100.0f))
		float _fRecoilInterpolationSpeedHipfireMaximum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable", ClampMin = 0.0f, ClampMax = 100.0f))
		float _fRecoilInterpolationSpeedAimingMinimum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable", ClampMin = 0.0f, ClampMax = 100.0f))
		float _fRecoilInterpolationSpeedAimingMaximum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable", ClampMin = 0.0f, ClampMax = 10.0f))
		float _fRecoilInterpolationTransitionSpeed = 3.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable"))
		bool _bRandomPitchRecoil = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable && _bRandomPitchRecoil", ClampMin = -15.0f, ClampMax = 15.0f))
		float _fRandomCameraHipfireRecoilPitchMinimum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable && _bRandomPitchRecoil", ClampMin = -15.0f, ClampMax = 15.0f))
		float _fRandomCameraHipfireRecoilPitchMaximum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable && _bRandomPitchRecoil", ClampMin = -15.0f, ClampMax = 15.0f))
		float _fRandomCameraAimingRecoilPitchMinimum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable && _bRandomPitchRecoil", ClampMin = -15.0f, ClampMax = 15.0f))
		float _fRandomCameraAimingRecoilPitchMaximum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable && !_bRandomPitchRecoil", ClampMin = -15.0f, ClampMax = 15.0f))
		float _fCameraHipfireRecoilPitch = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable && !_bRandomPitchRecoil", ClampMin = -15.0f, ClampMax = 15.0f))
		float _fCameraAimingRecoilPitch = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable"))
		bool _bRandomYawRecoil = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable && _bRandomYawRecoil", ClampMin = -15.0f, ClampMax = 15.0f))
		float _fRandomCameraHipfireRecoilYawMinimum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable && _bRandomYawRecoil", ClampMin = -15.0f, ClampMax = 15.0f))
		float _fRandomCameraHipfireRecoilYawMaximum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable && _bRandomYawRecoil", ClampMin = -15.0f, ClampMax = 15.0f))
		float _fRandomCameraAimingRecoilYawMinimum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable && _bRandomYawRecoil", ClampMin = -15.0f, ClampMax = 15.0f))
		float _fRandomCameraAimingRecoilYawMaximum = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable && !_bRandomYawRecoil", ClampMin = -15.0f, ClampMax = 15.0f))
		float _fCameraHipfireRecoilYaw = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Recoil",
		meta = (EditCondition = "!_bRecoilUsesDataTable && !_bRandomYawRecoil", ClampMin = -15.0f, ClampMax = 15.0f))
		float _fCameraAimingRecoilYaw = 0.0f;

	// Reload *********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Reload")
		E_ReloadType _eReloadType = E_ReloadType::eRT_Magazine;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Reload")
		bool _bAutomaticallyReloadOnEmptyMagazine = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Reload")
		bool _bSwapHandsWhenReloading = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Reload")
		float _fStartingTimeEjectMagazine = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Reload")
		float _fStartingTimeNewMagazine = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Reload")
		float _fStartingTimeChamberRound = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Reload")
		float _fTimeEjectMagazine = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Reload")
		float _fTimeNewMagazine = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Reload")
		float _fTimeChamberRound = 0.0f;

	// Spread Fire ****************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Spread Fire",
		meta = (EditCondition = "!_bUseDataTable"))
		int _iShotsFiredPerSpread = 1.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Spread Fire")
		bool _bIsUpdatingSpread = false;

	// UMG ************************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UMG")
		TSubclassOf<class UCrosshair> _CrosshairUMG;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UMG")
		UCrosshair* _CrosshairInstance = NULL;

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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	///////////////////////////////////////////////

	/**
	* @summary: Sets whether this firemode is currently misfired or not.
	*
	* @networking:	Runs on server
	*
	* @param:	bool Overheated
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetMisfired(bool Misfired);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetMagazineInWeapon(bool MagIn);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetWeaponAttached(AWeapon* Weapon);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		bool FireDelayComplete() { return _bFireDelayComplete; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		UAmmo* GetAmmoPool() { return _pAmmoPool; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void DetermineAmmoPool();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		bool IsFiring() { return _bIsFiring; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintSetter)
		void SetIsFiring(bool Firing);
	
	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetIsFiring(bool Firing);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Client, Reliable, WithValidation)
		void OwningClient_SetFireDelayComplete(bool Complete);

	// Aiming *********************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Aiming")
		float GetWeaponAimTime() { return _fAimTime; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Aiming")
		TArray<float> GetFovArray() { return _fFovAiming; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Aiming")
		bool IsAimDownSightEnabled() { return _bAimDownSightEnabled; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		FTransform GetOriginADS() { return _tAdsOriginHands; }

	// Animation ******************************************************************************************************************************

	/**
	* @summary:	Returns reference to an animation montage used on the weapon owner's first person mesh.
	*
	* @param:	E_HandAnimation AnimationEnum
	*
	* @return:	UAnimMontage*
	*/
	UFUNCTION()
		UAnimMontage* GetArmAnimation(E_HandAnimation AnimationEnum);

	///////////////////////////////////////////////

	/**
	* @summary:	Returns reference to an animation montage used on the weapon's mesh.
	*
	* @param:	E_GunAnimation AnimationEnum
	*
	* @return:	UAnimMontage*
	*/
	UFUNCTION()
		UAnimMontage* GetGunAnimation(E_GunAnimation AnimationEnum);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		bool GetWeaponMustBeChargedToFire() { return _bWeaponMustBeChargedToFire; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		bool IsWeaponFullyCharged() { return _bIsFullyCharged; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		E_FiringModeType GetFiringType() { return _FiringType; }

	// Contrail *******************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
		void Multicast_Unreliable_PlayThirdPersonContrail(FHitResult HitResult, FVector StartLocation);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Client, Unreliable, WithValidation)
		void OwningClient_Unreliable_PlayFirstPersonContrail(FHitResult HitResult, FVector StartLocation);

	// Damage *********************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Damage")
		float GetMaxRangeThreshold() { return _fMaxRangeThreshold; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		float GetDamageByPawnHitComponent(UPrimitiveComponent* ComponentHit);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void ApplyPointDamage(AActor* DamagedActor, float DamageToCause, USkeletalMeshComponent* SkCharWepMeshFirstP, FHitResult HitResult);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		float GetDamageFleshHead() { return _fDamageHead; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		float GetDamageFleshTorso() { return _fDamageTorso; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		float GetDamageFleshArms() { return _fDamageArms; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		float GetDamageFleshLegs() { return _fDamageLegs; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		float GetDamageFleshBase() { return _fDamageBase; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		float GetDamageShieldHead() { return _fDamageShieldHead; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		float GetDamageShieldBase() { return _fDamageShield; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		bool LosesDamageOverRange() { return _bLosesDamageOverDistance; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		UCurveFloat* GetDamageCurve() { return _cDamageDistanceCurve; }

	// Firing *********************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		void Fire(FHitResult hitResult, FVector CameraRotationXVector, USkeletalMeshComponent* SkCharWepMeshFirstP, USkeletalMeshComponent* SkCharWepMeshThirdP);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_FireProjectileTrace(APawn* Pawn, FHitResult hitResult, FVector CameraRotationXVector, USkeletalMeshComponent* SkCharWepMeshFirstP, USkeletalMeshComponent* SkCharWepMeshThirdP);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_FireProjectilePhysics(APawn* Pawn, FHitResult hitResult, FVector CameraRotationXVector, USkeletalMeshComponent* SkCharWepMeshFirstP, USkeletalMeshComponent* SkCharWepMeshThirdP);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Client, Unreliable, WithValidation)
		void OwningClient_Unreliable_DebugFireTrace(FVector StartPoint, FVector EndPoint);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		E_FiringTriggerType GetFiringTriggerType() { return _FiringTrigger; }
	
	// Heat ***********************************************************************************************************************************

	/**
	* @summary:	Sets whether this firemode is currently overheated or not.
	*
	* @networking:	Runs on server
	*
	* @param:	bool Overheated
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetOverheated(bool Overheated);

	// Impact Effects *************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		AImpactEffectManager* GetImpactEffectManager() { return _ImpactEffectManager; }

	// Muzzle Effect **************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
		void Multicast_Unreliable_PlayThirdPersonMuzzle(USkeletalMeshComponent* SkCharWepMeshThirdP);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Client, Unreliable, WithValidation)
		void OwningClient_Unreliable_PlayFirstPersonMuzzle(USkeletalMeshComponent* SkCharWepMeshFirstP);

	// Recoil *********************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(Client, Reliable, WithValidation)
		void OwningClient_Reliable_RecoilCamera();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Client, Reliable, WithValidation)
		void OwningClient_Reliable_StartRecoilInterpolation();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Client, Reliable, WithValidation)
		void OwningClient_Reliable_StopRecoilInterpolation();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void RecoilInterpolationUpdate();

	// Reload *********************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		E_ReloadType GetReloadType() { return _eReloadType; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		FTimerHandle GetReloadTimerHandle()	{ return _fReloadStageHandle; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetReloadComplete(bool ReloadComplete);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_EjectMagazine();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_NewMagazine();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_ChamberRound();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		bool GetReloadComplete() { return _bReloadComplete; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetReloadStage(E_ReloadStage ReloadStage);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		E_ReloadStage GetCurrentReloadState() { return _eReloadStage; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		float GetReloadStartingTimeEjectMagazine() { return _fStartingTimeEjectMagazine; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		float GetReloadStartingTimeNewMagazine() { return _fStartingTimeNewMagazine; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		float GetReloadStartingTimeChamberRound() { return _fStartingTimeChamberRound; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void SetReloadAnimationComplete(bool Complete) { _bReloadComplete = Complete; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		float GetReloadStartingTime();

	// Spread *********************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		float IncreaseSpread();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void StartDecreasingSpread() { _bCanDeductSpread = true; }

	///////////////////////////////////////////////

	/**
	* @summary:	Adds to the current projectile spread of the firemode.
	*
	* @networking:	Runs on server
	*
	* @param:	float Additive
	* @param:	float ClampMaximum
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_AddToSpread(float Additive, float ClampMinimum, float ClampMaximum);

	///////////////////////////////////////////////

	/**
	* @summary:	Subtracts to the current projectile spread of the firemode.
	*
	* @networking:	Runs on server
	*
	* @param:	float Subtractive
	* @param:	float ClampMinimum
	*
	* @return:	void
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_DeductFromSpread(float Subtractive, float ClampMinimum, float ClampMaximum);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void UpdateSpreadToMinimum(bool Aiming);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		float GetCurrentProjectileSpread() { return _fProjectileSpread; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		float GetSpreadHipMinimum() { return _fSpreadHipMinimum; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		float GetSpreadAimingMinimum() { return _fSpreadAimingMinimum; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		float GetSpreadHipMaximum() { return _fSpreadHipMaximum; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		float GetSpreadAimingMaximum() { return _fSpreadAimingMaximum; }

	// UMG ************************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "UMG")
		TSubclassOf<class UCrosshair> GetCrosshairClass() { return _CrosshairUMG; }

};