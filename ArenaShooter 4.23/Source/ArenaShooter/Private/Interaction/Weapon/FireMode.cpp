// Fill out your copyright notice in the Description page of Project Settings.

#include "FireMode.h"

#include "Ammo.h"
#include "BaseCharacter.h"
#include "BaseHUD.h"
#include "BasePlayerController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Crosshair.h"
#include "DrawDebugHelpers.h"
#include "ImpactEffectManager.h"
#include "kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "kismet/KismetSystemLibrary.h"
#include "Math/RandomStream.h"
#include "Particles/ParticleSystemComponent.h"
#include "Projectile.h"
#include "UnrealNetwork.h"
#include "UserWidget.h"

// Startup ********************************************************************************************************************************

/**
* @summary:	Sets default values for this component's properties.
*
* @return:	Constructor
*/
UFireMode::UFireMode()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Component replicates
	bReplicates = true;
}

///////////////////////////////////////////////

/**
* @summary:	Called when the game starts or when spawned.
*
* @return:	virtual void
*/
void UFireMode::BeginPlay()
{
	Super::BeginPlay();

	// Starting ammo
	if (_pAmmoPool == NULL) { DetermineAmmoPool(); }

	// Physical mesh properties based on starting ammo values
	///Server_Reliable_DetermineIfBulletShouldBeInChamber();
	///Server_Reliable_SetMagazineInWeapon(_iStartingReserveAmmo > 0);

	_fProjectileSpread = _fSpreadHipMinimum;

	// Get weapon parent attached
	if (_WeaponParentAttached == NULL) { _WeaponParentAttached = Cast<AWeapon>(GetOwner()); }
	if (_WeaponParentAttached != NULL)
	{
		// Create impact effect manager for this weapon
		if (_WeaponParentAttached->HasAuthority()) { Server_Reliable_CreateImpactManager(); }
	}

	// Initialize firing delay
	switch (_eDelayDirection)
	{
	case E_DelayDirectionType::eDDT_Additive:		{ _fFiringDelay = _fFiringDelayMinimum; break; }
	case E_DelayDirectionType::eDDT_Subtractive:	{ _fFiringDelay = _fFiringDelayMaximum; break; }
	default: break;
	}
}

///////////////////////////////////////////////

void UFireMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFireMode, _bCanTryToVentCooldown);
	DOREPLIFETIME(UFireMode, _bIsFiring);
	DOREPLIFETIME(UFireMode, _bIsMagazineInWeapon);
	DOREPLIFETIME(UFireMode, _bIsRoundInChamber);
	DOREPLIFETIME(UFireMode, _bReloadComplete); 
	DOREPLIFETIME(UFireMode, _eReloadStage);
	DOREPLIFETIME(UFireMode, _fHeatDecreaseMultiplier);
	DOREPLIFETIME(UFireMode, _fProjectileSpread);
	DOREPLIFETIME(UFireMode, _ImpactEffectManager);
}

///////////////////////////////////////////////

bool UFireMode::Server_Reliable_CreateImpactManager_Validate()
{ return true; }

void UFireMode::Server_Reliable_CreateImpactManager_Implementation()
{
	FVector location(0.0f, 0.0f, 0.0f);
	FRotator rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters spawnInfo;
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnInfo.Owner = _WeaponParentAttached;
	   
	_ImpactEffectManager = GetWorld()->SpawnActor<AImpactEffectManager>(_ImpactEffectManagerClass, location, rotation, spawnInfo);
}

// Current Frame **************************************************************************************************************************

/**
* @summary:	Called every frame.
*
* @param:	DeltaTime The delta of the frame.
*
* @return:	virtual void
*/
void UFireMode::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// First person muzzle position tick
	if (_pLocalMuzzleEffect != NULL)
	{
		AActor* owner = _WeaponParentAttached->GetPawnOwner();
		ABaseCharacter* character = Cast<ABaseCharacter>(owner);
		if (character != NULL)
		{
			// Primary weapon
			USkeletalMeshComponent* weaponskeletal = character->GetFirstPersonPrimaryWeaponMesh();
			if (weaponskeletal != NULL)
			{
				// Update position to muzzle socket position in the character's first person mesh
				FVector position = weaponskeletal->GetSocketLocation(_MuzzleSocketName);
				FRotator rotation = weaponskeletal->GetSocketRotation(_MuzzleSocketName);
				_pLocalMuzzleEffect->SetRelativeLocationAndRotation(position, rotation);
				///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("Updating local muzzle position"));

				/*	NOTE: The muzzle effect isn't being destroyed/re-pooled at the moment so the tick is constantly
				*	firing up to this point even after the muzzle effect has finished its cycle. */
			}
		}
	}

	// Third person muzzle position tick
	if (_pThirdPersonMuzzleEffect != NULL)
	{
		AActor* owner = _WeaponParentAttached->GetPawnOwner();
		ABaseCharacter* character = Cast<ABaseCharacter>(owner);
		if (character != NULL)
		{
			// Primary weapon
			USkeletalMeshComponent* weaponskeletal = character->GetThirdPersonPrimaryWeaponMesh();
			if (weaponskeletal != NULL)
			{
				// Update position to muzzle socket position in the character's third person mesh
				FVector position = weaponskeletal->GetSocketLocation(_MuzzleSocketName);
				FRotator rotation = weaponskeletal->GetSocketRotation(_MuzzleSocketName);
				_pThirdPersonMuzzleEffect->SetRelativeLocationAndRotation(position, rotation);
				///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("Updating third person muzzle position"));

				/*	NOTE: The muzzle effect isn't being destroyed/re-pooled at the moment so the tick is constantly
				*	firing up to this point even after the muzzle effect has finished its cycle. */
			}
		}
	}

	// Reduce spread if over minimum threshold
	bool instigatorAiming = _WeaponParentAttached->GetIsAiming();
	if (_bIsUpdatingSpread = _fProjectileSpread > UKismetMathLibrary::SelectFloat(_fSpreadHipMinimum, _fSpreadAimingMinimum, !instigatorAiming) && _bCanDeductSpread)
	{
		// Determine spread deduction amount (framerate safe)
		float subtractAmount = instigatorAiming ? _fSpreadAimingDecrease : _fSpreadHipDecrease;
		subtractAmount *= GetWorld()->GetDeltaSeconds();

		// Determine clamp values (Hipfire or Aiming?)
		float clampMin = instigatorAiming ? _fSpreadAimingMinimum : _fSpreadHipMinimum;
		float clampMax = instigatorAiming ? _fSpreadAimingMaximum : _fSpreadHipMaximum;

		// Deduct from spread
		if (_WeaponParentAttached->HasAuthority())
		{
			_fProjectileSpread -= subtractAmount;
			_fProjectileSpread = FMath::Clamp(_fProjectileSpread, clampMin, clampMax);
		}
		else
		{ Server_Reliable_DeductFromSpread(subtractAmount, clampMin, clampMax); }
		
		///FString _Message = TEXT("Spread is: ") + FString::SanitizeFloat(_fProjectileSpread);
		///GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Orange, _Message);
	}

	// Reload complete?
	if (_bReloadComplete) { Server_Reliable_SetReloadComplete(false); }

	// Tick recoil interpolation
	if (_bUpdateRecoilInterpolation) { RecoilInterpolationUpdate(); }
}

///////////////////////////////////////////////

/**
* @summary:	Sets whether this firemode is currently misfired or not.
*
* @networking:	Runs on server
*
* @param:	bool Overheated
*
* @return:	void
*/
bool UFireMode::Server_Reliable_SetMisfired_Validate(bool Misfired)
{ return true; }

void UFireMode::Server_Reliable_SetMisfired_Implementation(bool Misfired)
{ _bIsMisfired = Misfired; }

///////////////////////////////////////////////

/**
* @summary:	Sets whether there is a magazine in or not for this firemode.
*
* @networking:	Runs on server
*
* @param:	bool MagIn
*
* @return:	void
*/
bool UFireMode::Server_Reliable_SetMagazineInWeapon_Validate(bool MagIn)
{ return true; }

void UFireMode::Server_Reliable_SetMagazineInWeapon_Implementation(bool MagIn)
{ _bIsMagazineInWeapon = MagIn; }

///////////////////////////////////////////////

bool UFireMode::Server_Reliable_SetWeaponAttached_Validate(AWeapon* Weapon)
{ return true; }

void UFireMode::Server_Reliable_SetWeaponAttached_Implementation(AWeapon* Weapon)
{ _WeaponParentAttached = Weapon; }

///////////////////////////////////////////////

void UFireMode::SetIsFiring(bool Firing)
{
	// Started firing
	if (!_bIsFiring && Firing == true) { OwningClient_Reliable_StartRecoilInterpolation(); }

	// Stopped firing
	if (_bIsFiring && Firing == false) { OwningClient_Reliable_StopRecoilInterpolation(); }

	// Set _bIsFiring to the new value (network replicated)
	if (_WeaponParentAttached->Role == ROLE_Authority)
	{ _bIsFiring = Firing; } else
	{ Server_Reliable_SetIsFiring(Firing); }
}

bool UFireMode::Server_Reliable_SetIsFiring_Validate(bool Firing)
{ return true; }

void UFireMode::Server_Reliable_SetIsFiring_Implementation(bool Firing)
{
	SetIsFiring(Firing);
}

// Animation ******************************************************************************************************************************

/*
*
*/
void UFireMode::DetermineAmmoPool()
{
	if (_WeaponParentAttached == NULL) { _WeaponParentAttached = Cast<AWeapon>(GetOwner()); }
	if (_WeaponParentAttached != NULL)
	{
		TArray<UAmmo*> pool = _WeaponParentAttached->GetAmmoPool();
		for (int i = 0; i < pool.Num(); i++)
		{
			if (_iAmmoChannel == pool[i]->GetAmmoChannel())
			{
				_pAmmoPool = pool[i];
				break;
			}
		}
	}
}

///////////////////////////////////////////////

/**
* @summary:	Returns reference to an animation montage used on the weapon owner's first person mesh.
*
* @param:	E_HandAnimation AnimationEnum
*
* @return:	UAnimMontage*
*/
UAnimMontage* UFireMode::GetArmAnimation(E_HandAnimation AnimationEnum)
{
	switch (AnimationEnum)
	{
	case E_HandAnimation::eHA_FirstPickup:				return _AnimationMontageListHands._Anim_FirstPickup;
	case E_HandAnimation::eHA_Equip:					return _AnimationMontageListHands._Anim_Equip;
	case E_HandAnimation::eHA_Unequip:					return _AnimationMontageListHands._Anim_Unequip;
	case E_HandAnimation::eHA_Inspect:					return _AnimationMontageListHands._Anim_Inspect;
	case E_HandAnimation::eHA_ReloadFullNotEmpty:		return _AnimationMontageListHands._Anim_ReloadFullNotEmpty;
	case E_HandAnimation::eHA_ReloadFullEmpty:			return _AnimationMontageListHands._Anim_ReloadFullEmpty;
	case E_HandAnimation::eHA_ReloadDuelRight:			return _AnimationMontageListHands._Anim_ReloadDuelRight;
	case E_HandAnimation::eHA_ReloadDuelLeft:			return _AnimationMontageListHands._Anim_ReloadDuelLeft;
	case E_HandAnimation::eHA_FireProjectileHipfire:	return _AnimationMontageListHands._Anim_FireProjectileHipfire;
	case E_HandAnimation::eHA_FireProjectileAiming:		return _AnimationMontageListHands._Anim_FireProjectileADS;
	case E_HandAnimation::eHA_ProjectileMisfire:		return _AnimationMontageListHands._Anim_ProjectileMisfire;
	case E_HandAnimation::eHA_ProjectilePump:			return _AnimationMontageListHands._Anim_PumpProjectile;
	case E_HandAnimation::eHA_LoopingReloadEnter:		return _AnimationMontageListHands._Anim_LoopingReloadEnter;
	case E_HandAnimation::eHA_LoopingReloadLoop:		return _AnimationMontageListHands._Anim_LoopingReload;
	case E_HandAnimation::eHA_LoopingReloadExit:		return _AnimationMontageListHands._Anim_LoopingReloadExit;
	default: break;
	}
	return NULL;
}

///////////////////////////////////////////////

/**
* @summary:	Returns reference to an animation montage used on the weapon's mesh.
*
* @param:	E_GunAnimation AnimationEnum
*
* @return:	UAnimMontage*
*/
UAnimMontage* UFireMode::GetGunAnimation(E_GunAnimation AnimationEnum)
{
	switch (AnimationEnum)
	{
	case E_GunAnimation::eGA_FirstPickup:				return _AnimationMontageListWeapon._Anim_FirstPickup;
	case E_GunAnimation::eGA_Inspect:					return _AnimationMontageListWeapon._Anim_Inspect;
	case E_GunAnimation::eGA_ReloadFullNotEmpty:		return _AnimationMontageListWeapon._Anim_ReloadFullNotEmpty;
	case E_GunAnimation::eGA_ReloadFullEmpty:			return _AnimationMontageListWeapon._Anim_ReloadFullEmpty;
	case E_GunAnimation::eGA_ReloadDuelRight:			return _AnimationMontageListWeapon._Anim_ReloadDuelRight;
	case E_GunAnimation::eGA_ReloadDuelLeft:			return _AnimationMontageListWeapon._Anim_ReloadDuelLeft;
	case E_GunAnimation::eGA_FireProjectileHipfire:		return _AnimationMontageListWeapon._Anim_FireProjectileHipfire;
	case E_GunAnimation::eGA_FireProjectileAiming:		return _AnimationMontageListWeapon._Anim_FireProjectileADS;
	case E_GunAnimation::eGA_ProjectileMisfire:			return _AnimationMontageListWeapon._Anim_ProjectileMisfire;
	case E_GunAnimation::eGA_ProjectilePump:			return _AnimationMontageListWeapon._Anim_PumpProjectile;
	case E_GunAnimation::eGA_LoopingReloadEnter:		return _AnimationMontageListWeapon._Anim_LoopingReloadEnter;
	case E_GunAnimation::eGA_LoopingReloadLoop:			return _AnimationMontageListWeapon._Anim_LoopingReload;
	case E_GunAnimation::eGA_LoopingReloadExit:			return _AnimationMontageListWeapon._Anim_LoopingReloadExit;
	default: break;
	}
	return NULL;
}

// Contrail *******************************************************************************************************************************

/*
*
*/
bool UFireMode::Multicast_Unreliable_PlayThirdPersonContrail_Validate(FHitResult HitResult, FVector StartLocation)
{ return true; }

void UFireMode::Multicast_Unreliable_PlayThirdPersonContrail_Implementation(FHitResult HitResult, FVector StartLocation)
{
	// Play contrail effect on all non-local clients (owner no see)
	if (!_WeaponParentAttached->GetPawnOwner()->IsLocallyControlled())
	{
		if (_pContrailParticleSystem != NULL && _WeaponParentAttached->Role < ROLE_Authority)
		{
			FRotator contrailRotation = UKismetMathLibrary::MakeRotFromX(HitResult.Normal);

			// Spawn contrail
			UParticleSystemComponent* particleTP = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _pContrailParticleSystem, StartLocation, contrailRotation, FVector(1.0f, 1.0f, 1.0f));

			// Rotate contrail effect to face impact point
			if (HitResult.IsValidBlockingHit())
			{
				particleTP->SetVectorParameter(_fContrailName, HitResult.ImpactPoint);
			} else
			{
				// Contrail didn't hit anything >> get facing direction of the pawn
				FVector dir = FVector::ZeroVector;
				AActor* owningActor = _WeaponParentAttached->GetPawnOwner();
				dir = owningActor->GetActorForwardVector();

				particleTP->SetVectorParameter(_fContrailName, StartLocation + (dir * _fMaxRangeThreshold));
			}
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
bool UFireMode::OwningClient_Unreliable_PlayFirstPersonContrail_Validate(FHitResult HitResult, FVector StartLocation)
{ return true; }

void UFireMode::OwningClient_Unreliable_PlayFirstPersonContrail_Implementation(FHitResult HitResult, FVector StartLocation)
{
	// Play contrail effect
	if (_pContrailParticleSystem != NULL)
	{
		FRotator contrailRotation = UKismetMathLibrary::MakeRotFromX(HitResult.Normal);

		// Spawn contrail
		UParticleSystemComponent* particleFP = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _pContrailParticleSystem, StartLocation, contrailRotation, FVector(1.0f, 1.0f, 1.0f));

		// Rotate contrail effect to face impact point
		if (HitResult.IsValidBlockingHit())
		{
			particleFP->SetVectorParameter(_fContrailName, HitResult.ImpactPoint);
		} else
		{
			// Contrail didn't hit anything >> get facing direction of the pawn
			FVector dir = FVector::ZeroVector;
			AActor* owningActor = _WeaponParentAttached->GetPawnOwner();
			dir = owningActor->GetActorForwardVector();

			particleFP->SetVectorParameter(_fContrailName, StartLocation + (dir * _fMaxRangeThreshold));
		}
		
	}
}

// Damage *********************************************************************************************************************************

/*
*
*/
float UFireMode::GetDamageByPawnHitComponent(UPrimitiveComponent* ComponentHit)
{
	float damage = _fDamageTorso;
	FString compName = UKismetSystemLibrary::GetObjectName(ComponentHit);

	// TEMPORARY -> Needs to be updated to a more versatile hitbox system that incorporates characters/vehicles/damagable static objects
	if (compName == TEXT("_HitBox_Head"))				{ damage = _fDamageHead; }
	else if (compName == TEXT("_HitBox_TorsoUpper"))	{ damage = _fDamageTorso; } 
	else if (compName == TEXT("_HitBox_TorsoLower"))	{ damage = _fDamageTorso; }

	// Arms
	else if (compName == TEXT("_HitBox_RightShoulder"))	{ damage = _fDamageArms; }
	else if (compName == TEXT("_HitBox_RightForearm"))	{ damage = _fDamageArms; }
	else if (compName == TEXT("_HitBox_RightHand"))		{ damage = _fDamageArms; }
	else if (compName == TEXT("_HitBox_LeftShoulder"))	{ damage = _fDamageArms; }
	else if (compName == TEXT("_HitBox_LeftForearm"))	{ damage = _fDamageArms; }
	else if (compName == TEXT("_HitBox_LeftHand"))		{ damage = _fDamageArms; }

	// Legs
	else if (compName == TEXT("_HitBox_RightThigh"))	{ damage = _fDamageLegs; }
	else if (compName == TEXT("_HitBox_RightShin"))		{ damage = _fDamageLegs; }
	else if (compName == TEXT("_HitBox_LeftThigh"))		{ damage = _fDamageLegs; }
	else if (compName == TEXT("_HitBox_LeftShin"))		{ damage = _fDamageLegs; }	

	return damage;
}

///////////////////////////////////////////////

/*
*
*/
void UFireMode::ApplyPointDamage(AActor* DamagedActor, float DamageToCause, USkeletalMeshComponent* SkCharWepMeshFirstP, FHitResult HitResult)
{
	// Get hit direction for the damaged character
	FVector startLoc = SkCharWepMeshFirstP->GetSocketLocation("MuzzleLaunchPoint");
	FVector endLoc = DamagedActor->GetActorLocation();
	FVector hitDirection = endLoc - startLoc;
	UKismetMathLibrary::Vector_Normalize(hitDirection);

	// Get controller instigator
	AActor* owner = _WeaponParentAttached->GetOwner();
	AController* instigator = owner->GetInstigatorController();

	// Apply point damage from raycast
	UGameplayStatics::ApplyPointDamage(DamagedActor, DamageToCause, hitDirection, HitResult, instigator, _WeaponParentAttached, NULL);
}

// Firing *********************************************************************************************************************************

/*
*
*/
void UFireMode::Fire(FHitResult HitResult, FVector CameraRotationXVector, USkeletalMeshComponent* SkCharWepMeshFirstP, USkeletalMeshComponent* SkCharWepMeshThirdP)
{
	// Sanity check
	if (_WeaponParentAttached == NULL) { return; }
	if (_pAmmoPool == NULL) { DetermineAmmoPool(); return; }
	if (!FireDelayComplete()) { return; }
	
	bool shouldReturn = false;
	if (_bIsOverheated)
	{

	}
	else
	{		
		switch (_pAmmoPool->GetAmmoType())
		{
		case E_AmmoType::eAT_Magazine:
		{
			// Ammo in magazine?
			if (_pAmmoPool->GetMagazineAmmo() > 0)
			{
				// Pump action firing?
				if (_bEachFireRequiresChambering)
				{
					// Has been chambered?
					if (_bHasBeenChambered && _bIsRoundInChamber)
					{
						// Fire projectile (per spread)
						for (int i = 0; i < _iShotsFiredPerSpread; i++)
						{ FireProjectile(HitResult, CameraRotationXVector, SkCharWepMeshFirstP, SkCharWepMeshThirdP); }
					}
				}

				// Not pump action
				else
				{
					// Round in chamber?
					if (_pAmmoPool->IsRoundInChamber())
					{
						// Fire projectile (per spread)
						for (int i = 0; i < _iShotsFiredPerSpread; i++)
						{ FireProjectile(HitResult, CameraRotationXVector, SkCharWepMeshFirstP, SkCharWepMeshThirdP); }
					}
					else
					{
						///FString Message = TEXT("ROUND ISNT IN CHAMBER");
						///GEngine->AddOnScreenDebugMessage(20, 5.0f, FColor::Red, Message);

						// Chamber new round
						AActor* pawnOwner = _WeaponParentAttached->GetPawnOwner();
						ABaseCharacter* character = Cast<ABaseCharacter>(pawnOwner);

						// Play animation
						uint8 handAnim = (uint8)E_HandAnimation::eHA_ReloadFullEmpty;
						uint8 gunAnim = (uint8)E_GunAnimation::eGA_ReloadFullEmpty;
						character->OwningClient_PlayPrimaryWeaponFPAnimation(character->GetGlobalReloadPlayRate(), false, true, handAnim, _fStartingTimeChamberRound, true, gunAnim, _fStartingTimeChamberRound);

						shouldReturn = true;
					}
				}
			}

			// Empty mag
			else
			{
				// Auto reload weapon?
				if (_pAmmoPool->GetReserveAmmo() > 0 && _bAutomaticallyReloadOnEmptyMagazine)
				{
					// Primary or secondary weapon?
					if (_WeaponParentAttached->IsOwnersPrimaryWeapon())
					{
						// Try to reload primary weapon
						ABaseCharacter* baseChar = Cast<ABaseCharacter>(_WeaponParentAttached->GetOwner());
						baseChar->InputReloadPrimaryWeapon();
					}
					if (_WeaponParentAttached->IsOwnersSecondaryWeapon())
					{
						// Try to reload secondary weapon
						ABaseCharacter* baseChar = Cast<ABaseCharacter>(_WeaponParentAttached->GetOwner());
						baseChar->InputReloadSecondaryWeapon();					
					}

					shouldReturn = true;
				}
			}

			break;
		}
		case E_AmmoType::eAT_Battery:
		{
			if (_pAmmoPool->GetBatteryAmmo() > 0)
			{
				// Battery ammo is still greater than the potential misfiring threshold
				if (_pAmmoPool->GetBatteryAmmo() > _pAmmoPool->GetBatteryMisfireThreshold())
				{
					// Fire projectile
					Server_Reliable_SetMisfired(false); // Just to be safe
					FireProjectile(HitResult, CameraRotationXVector, SkCharWepMeshFirstP, SkCharWepMeshThirdP);
				}

				// Can potentially misfire the weapon
				else
				{
					// Misfire (uses weighted chance so less ammo means greater chance of misfiring)
					bool misfire = !UKismetMathLibrary::RandomBoolWithWeight(_pAmmoPool->GetBatteryAmmo() / _pAmmoPool->GetBatteryMisfireThreshold());
					float weight = 1.0f - (_pAmmoPool->GetBatteryAmmo() / _pAmmoPool->GetBatteryMisfireThreshold());
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, TEXT("Misfire Weight: ") + FString::SanitizeFloat(weight));
					if (misfire)
					{
						// Misfire
						///Server_Reliable_SetMisfired(true);

						// Start fire delay for re-chambering round (this method is also used to "un-misfire" the weapon
						///FTimerDelegate fireDelayDelegate;
						///fireDelayDelegate.BindUFunction(this, FName("OwningClient_SetFireDelayComplete"), true);
						///GetWorld()->GetTimerManager().SetTimer(_fFireDelayHandle, fireDelayDelegate, 1.0f, false, _fFiringDelay);

						///shouldReturn = true;
					}

					// Didn't misfire
					else
					{
						// Fire projectile
						Server_Reliable_SetMisfired(false);
						FireProjectile(HitResult, CameraRotationXVector, SkCharWepMeshFirstP, SkCharWepMeshThirdP);
					}
				}
			} else
			{ shouldReturn = true; }

			break;
		}

		default: break;
		}
	}

	if (shouldReturn) { return; }
	
	SetIsFiring(true);

	// Deduct ammo and re-chamber new bullet
	_pAmmoPool->Server_Reliable_DeductAmmo(1);
	OwningClient_SetFireDelayComplete(false);

	// Empty mag? Means no round in the chamber
	if (_pAmmoPool->GetMagazineAmmo() == 0 && _pAmmoPool->IsRoundInChamber() == true) { _pAmmoPool->Server_Reliable_SetRoundInChamber(false); }

	// Start fire delay for re-chambering round
	FTimerDelegate fireDelayDelegate;
	fireDelayDelegate.BindUFunction(this, FName("OwningClient_SetFireDelayComplete"), true);
	GetWorld()->GetTimerManager().SetTimer(_fFireDelayHandle, fireDelayDelegate, 1.0f, false, _fFiringDelay);

	// Update spread
	IncreaseSpread();

	// Update recoil
	OwningClient_Reliable_RecoilCamera();

	// Start decreasing spread on the "repeating/automatic" styled firing types
	if (_FiringType == E_FiringModeType::eFMT_FullAuto || _FiringType == E_FiringModeType::eFMT_Burst && !_bIsUpdatingSpread)
	{
		// Start projectile spread delay
		FTimerDelegate projectileSpreadDelegate;
		projectileSpreadDelegate.BindUFunction(this, FName("StartDecreasingSpread"));
		GetWorld()->GetTimerManager().SetTimer(_fProjectileSpreadHandle, projectileSpreadDelegate, 1.0f, false, _fSpreadDecreaseInitialDelay);
	}
	   
	// Play muzzle effects
	OwningClient_Unreliable_PlayFirstPersonMuzzle(SkCharWepMeshFirstP);
	Multicast_Unreliable_PlayThirdPersonMuzzle(SkCharWepMeshThirdP);

	// Play firing animations
	ABaseCharacter* character = Cast<ABaseCharacter>(_WeaponParentAttached->GetPawnOwner());
	if (character != NULL)
	{
		// Enum animation to byte (aim or hipfire?)
		uint8 handByte;
		uint8 gunByte;
		if (_bAimDownSightEnabled && character->IsAiming())
		{
			handByte = (uint8)E_HandAnimation::eHA_FireProjectileAiming;
			gunByte = (uint8)E_GunAnimation::eGA_FireProjectileAiming;
		} else
		{
			handByte = (uint8)E_HandAnimation::eHA_FireProjectileHipfire;
			gunByte = (uint8)E_GunAnimation::eGA_FireProjectileHipfire;
		}		

		character->OwningClient_PlayPrimaryWeaponFPAnimation(1.0f, false, true, handByte, 0.0f, true, gunByte, 0.0f);
	}

	// Camera shake
	bool aiming = character->IsAiming();
	TSubclassOf<UCameraShake> cameraShakeClass = aiming ? _AimingRecoilCamShakeClass : _HipfireRecoilCamShakeClass;
	character->OwningClient_PlayCameraShake(cameraShakeClass, 1.0f);

	// Gamepad rumble(s)
	APlayerController* playerController = Cast<APlayerController>(character->GetController());
	if (playerController == NULL) { return; }
	ABasePlayerController* arenaController = Cast<ABasePlayerController>(playerController);
	if (arenaController == NULL) { return; }

	// Left small
	bool affectsLeftSmall = false;
	if (_WeaponParentAttached->CanBeDuelWielded())
	{
		// Left small > duel primary
		if (_WeaponParentAttached->IsOwnersPrimaryWeapon())
		{ affectsLeftSmall = _bGamepadRumbleFiringAffectsLeftSmall_DuelPrimary; }

		// Left small > duel secondary
		else if (_WeaponParentAttached->IsOwnersSecondaryWeapon())
		{ affectsLeftSmall = _bGamepadRumbleFiringAffectsLeftSmall_DuelSecondary; }
	}

	// Left small > two-hand weapon
	else
	{ affectsLeftSmall = _bGamepadRumbleFiringAffectsLeftSmall_Primary; }

	// Left large
	bool affectsLeftLarge = false;
	if (_WeaponParentAttached->CanBeDuelWielded())
	{
		// Left large > duel primary
		if (_WeaponParentAttached->IsOwnersPrimaryWeapon())
		{ affectsLeftLarge = _bGamepadRumbleFiringAffectsLeftLarge_DuelPrimary; }

		// Left large > duel secondary
		else if (_WeaponParentAttached->IsOwnersSecondaryWeapon())
		{ affectsLeftLarge = _bGamepadRumbleFiringAffectsLeftLarge_DuelSecondary; }
	}

	// Left large > two-hand weapon
	else
	{ affectsLeftLarge = _bGamepadRumbleFiringAffectsLeftLarge_Primary; }

	// Right small
	bool affectsRightSmall = false;
	if (_WeaponParentAttached->CanBeDuelWielded())
	{
		// Right small > duel primary
		if (_WeaponParentAttached->IsOwnersPrimaryWeapon())
		{ affectsRightSmall = _bGamepadRumbleFiringAffectsRightSmall_DuelPrimary; }

		// Right small > duel secondary
		else if (_WeaponParentAttached->IsOwnersSecondaryWeapon())
		{ affectsRightSmall = _bGamepadRumbleFiringAffectsRightSmall_DuelSecondary; }
	}

	// Right small > two-hand weapon
	else
	{ affectsRightSmall = _bGamepadRumbleFiringAffectsRightSmall_Primary; }

	// Right large
	bool affectsRightLarge = false;
	if (_WeaponParentAttached->CanBeDuelWielded())
	{
		// Right large > duel primary
		if (_WeaponParentAttached->IsOwnersPrimaryWeapon())
		{ affectsRightLarge = _bGamepadRumbleFiringAffectsRightLarge_DuelPrimary; }

		// Right large > duel secondary
		else if (_WeaponParentAttached->IsOwnersSecondaryWeapon())
		{ affectsRightLarge = _bGamepadRumbleFiringAffectsRightLarge_DuelSecondary; }
	}

	// Right large > two-hand weapon
	else
	{ affectsRightLarge = _bGamepadRumbleFiringAffectsRightLarge_Primary; }

	// Start rumble
	arenaController->GamepadRumble(_fGamepadRumbleFiringIntensity, _fGamepadRumbleFiringDuration, affectsLeftLarge, affectsLeftSmall, affectsRightLarge, affectsRightSmall);
}

///////////////////////////////////////////////

/*
*
*/
void UFireMode::FireProjectile(FHitResult hitResult, FVector CameraRotationXVec, USkeletalMeshComponent* SkCharWepMeshFirstP, USkeletalMeshComponent* SkCharWepMeshThirdP)
{
	switch (_eProjectileType)
	{
	case E_ProjectileType::ePT_Physics:
	{
		// Spawn physics projectile
		Server_Reliable_FireProjectilePhysics(Cast<APawn>(_WeaponParentAttached->GetOwner()), hitResult, CameraRotationXVec, SkCharWepMeshFirstP, SkCharWepMeshThirdP);
		break;
	}

	case E_ProjectileType::ePT_Raycast:
	{
		// Fire hitscan projectile
		Server_Reliable_FireProjectileTrace(Cast<APawn>(_WeaponParentAttached->GetOwner()), hitResult, CameraRotationXVec, SkCharWepMeshFirstP, SkCharWepMeshThirdP);
		break;
	}
	default: break;
	}
}

///////////////////////////////////////////////

bool UFireMode::Server_Reliable_FireProjectileTrace_Validate(APawn* Pawn, FHitResult hitResult, FVector CameraRotationXVec, USkeletalMeshComponent* SkCharWepMeshFirstP, USkeletalMeshComponent* SkCharWepMeshThirdP)
{ return true; }

void UFireMode::Server_Reliable_FireProjectileTrace_Implementation(APawn* Pawn, FHitResult hitResult, FVector CameraRotationXVec, USkeletalMeshComponent* SkCharWepMeshFirstP, USkeletalMeshComponent* SkCharWepMeshThirdP)
{
	FHitResult traceHitOut;
	FVector muzzleLaunchPointFP = SkCharWepMeshFirstP->GetSocketLocation("MuzzleLaunchPoint");
	FVector muzzleLaunchPointTP = SkCharWepMeshThirdP->GetSocketLocation("MuzzleLaunchPoint");
	
	// Get rotation X Vector
	///FVector cameraRotationXVec = ProjectileTransform.GetRotation().Vector();

	// Get trace end
	FVector traceEnd = FVector::ZeroVector;
	if (hitResult.IsValidBlockingHit())
	{ traceEnd = hitResult.ImpactPoint; }
	else 
	{ traceEnd = CameraRotationXVec * 20000.0f; }

	// Fire projectile trace
	ECollisionChannel collisionChannel = ECollisionChannel::ECC_GameTraceChannel15;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(Pawn);
	queryParams.bReturnPhysicalMaterial = true;
	GetWorld()->LineTraceSingleByChannel(traceHitOut, muzzleLaunchPointFP, traceEnd, collisionChannel, queryParams);
	///DrawDebugLine(GetWorld(), muzzleLaunchPointFP, traceEnd, FColor::Blue, false, 1.0f);
	///OwningClient_Unreliable_DebugFireTrace(muzzleLaunchPointFP, traceEnd);
	
	// Passed max distance threshold?
	if (traceHitOut.Distance <= _fMaxRangeThreshold)
	{
		// Hit something
		if (traceHitOut.IsValidBlockingHit())
		{
			// Can the actor be damaged?
			if (traceHitOut.GetActor() != NULL)
			{
				if (traceHitOut.GetActor()->bCanBeDamaged)
				{
					float damageToCauseByhitscan = _fDamageBase;

					// Does the HitActor inherit from ABaseCharacter class?
					ABaseCharacter* character = Cast<ABaseCharacter>(traceHitOut.Actor);
					if (character != NULL)
					{
						// Character is alive
						if (character->IsAlive())
						{
							// If shield is depleted
							if (character->IsShieldDepleted())
							{
								// Get damage by hit component
								damageToCauseByhitscan = GetDamageByPawnHitComponent(traceHitOut.GetComponent());

								// Damage falloff?
								if (_bLosesDamageOverDistance)
								{
									if (_cDamageDistanceCurve != NULL)
									{
										// Multiply damage against the curve (I normalize it to a range of [0f - 1f] as a precaution
										float multiplier = _cDamageDistanceCurve->GetFloatValue(traceHitOut.Distance);
										UKismetMathLibrary::NormalizeToRange(multiplier, 0.0f, 1.0f);
										damageToCauseByhitscan *= multiplier;

										// Damage character
										ApplyPointDamage(character, damageToCauseByhitscan, SkCharWepMeshFirstP, traceHitOut);
									}
								}

								// No damage falloff -> ApplyPointDamage()
								else
								{ ApplyPointDamage(character, damageToCauseByhitscan, SkCharWepMeshFirstP, traceHitOut); }
							}

							// Shield is NOT depleted
							else
							{
								// Get damage by hit component
								FString compName = UKismetSystemLibrary::GetObjectName(traceHitOut.GetComponent());
								bool hitHead = compName == TEXT("_HitBox_Head");
								damageToCauseByhitscan = UKismetMathLibrary::SelectFloat(_fDamageShieldHead, _fDamageShield, hitHead);

								// Damage falloff?
								if (_bLosesDamageOverDistance)
								{
									if (_cDamageDistanceCurve != NULL)
									{
										// Multiply damage against the curve (I normalize it to a range of [0f - 1f] as a precaution
										float multiplier = _cDamageDistanceCurve->GetFloatValue(traceHitOut.Distance);
										UKismetMathLibrary::NormalizeToRange(multiplier, 0.0f, 1.0f);
										damageToCauseByhitscan *= multiplier;

										// Damage character
										ApplyPointDamage(character, damageToCauseByhitscan, SkCharWepMeshFirstP, traceHitOut);
									}
								}

								// No damage falloff -> ApplyPointDamage()
								else
								{ ApplyPointDamage(character, damageToCauseByhitscan, SkCharWepMeshFirstP, traceHitOut); }

							}

							// Show hitmarker
							if (_CrosshairInstance != NULL) { _CrosshairInstance->GetHitMarkerDelegate().Broadcast(); }							
						}

						// Character is dead
						else
						{

						}
					}

					// HitActor does NOT inherit from ABaseCharacter class
					else
					{

					}
				}
			}
			
			// Play impact effects
			if (_ImpactEffectManager != NULL) { _ImpactEffectManager->Server_Reliable_SpawnImpactEffect(traceHitOut); }
		}
	}
	
	// Play contrail effects
	OwningClient_Unreliable_PlayFirstPersonContrail(traceHitOut, muzzleLaunchPointFP);

	// Third person should be visible by everyone except the owner
	Multicast_Unreliable_PlayThirdPersonContrail(traceHitOut, muzzleLaunchPointTP);
}

///////////////////////////////////////////////

bool UFireMode::OwningClient_Unreliable_DebugFireTrace_Validate(FVector StartPoint, FVector EndPoint)
{ return true; }

void UFireMode::OwningClient_Unreliable_DebugFireTrace_Implementation(FVector StartPoint, FVector EndPoint)
{ DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Blue, false, 1.0f); }

///////////////////////////////////////////////

bool UFireMode::Server_Reliable_FireProjectilePhysics_Validate(APawn* Pawn, FHitResult hitResult, FVector CameraRotationXVec, USkeletalMeshComponent* SkCharWepMeshFirstP, USkeletalMeshComponent* SkCharWepMeshThirdP)
{ return true; }

void UFireMode::Server_Reliable_FireProjectilePhysics_Implementation(APawn* Pawn, FHitResult hitResult, FVector CameraRotationXVec, USkeletalMeshComponent* SkCharWepMeshFirstP, USkeletalMeshComponent* SkCharWepMeshThirdP)
{
	if (_uProjectileClass == NULL) { return; }

	// Spawn projectile class at muzzle launch point
	FVector muzzleLocation = SkCharWepMeshFirstP->GetSocketLocation("MuzzleLaunchPoint");
	///FVector cameraRotationXVec = ProjectileTransform.GetRotation().Vector();
	FRotator muzzleRotation = CameraRotationXVec.Rotation();
	
	FActorSpawnParameters spawnInfo;
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AProjectile* projectile = GetWorld()->SpawnActor<AProjectile>(_uProjectileClass, muzzleLocation, muzzleRotation, spawnInfo);

	projectile->SetWeapon(_WeaponParentAttached);
	projectile->Instigator = _WeaponParentAttached->GetPawnOwner();
	projectile->Init(hitResult.GetActor());
}

///////////////////////////////////////////////

bool UFireMode::OwningClient_SetFireDelayComplete_Validate(bool Complete)
{ return true; }

void UFireMode::OwningClient_SetFireDelayComplete_Implementation(bool Complete)
{
	_bFireDelayComplete = Complete;
	if (Complete) { Server_Reliable_SetMisfired(false); }
}

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
bool UFireMode::Server_Reliable_SetOverheated_Validate(bool Overheated)
{ return true; }

void UFireMode::Server_Reliable_SetOverheated_Implementation(bool Overheated)
{
	_bIsOverheated = Overheated;
	if (Overheated)
	{
		_fHeatDecreaseMultiplier = 1.0f;
		auto weaponAttached = Cast<AWeapon>(this->GetOwner());
		if (weaponAttached != NULL)
		{
			// weaponattached-pawnowner-stoptryingtoshootprimaryandsecondaryonclient
		}
	} else
	{
		_bCanTryToVentCooldown = true;
		_fHeatDecreaseMultiplier = 1.0f;
	}
}

// Muzzle Effect **************************************************************************************************************************

/*
*
*/
bool UFireMode::Multicast_Unreliable_PlayThirdPersonMuzzle_Validate(USkeletalMeshComponent* SkCharWepMeshThirdP)
{ return true; }

void UFireMode::Multicast_Unreliable_PlayThirdPersonMuzzle_Implementation(USkeletalMeshComponent* SkCharWepMeshThirdP)
{
	// Play contrail effect on all non-local clients (owner no see)
	if (!_WeaponParentAttached->GetPawnOwner()->IsLocallyControlled())
	{
		if (_pMuzzleEffectParticleSystem != NULL && _WeaponParentAttached->Role < ROLE_Authority)
		{
			// Spawn muzzle effect
			FVector position = SkCharWepMeshThirdP->GetSocketLocation(_MuzzleSocketName);
			FRotator rotation = SkCharWepMeshThirdP->GetSocketRotation(_MuzzleSocketName);
			_pThirdPersonMuzzleEffect = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _pMuzzleEffectParticleSystem, position, rotation, FVector(1.0f, 1.0f, 1.0f));
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
bool UFireMode::OwningClient_Unreliable_PlayFirstPersonMuzzle_Validate(USkeletalMeshComponent* SkCharWepMeshFirstP)
{ return true; }

void UFireMode::OwningClient_Unreliable_PlayFirstPersonMuzzle_Implementation(USkeletalMeshComponent* SkCharWepMeshFirstP)
{
	// Sanity check
	if (_pMuzzleEffectParticleSystem != NULL)
	{
		// Spawn muzzle effect
		FVector position = SkCharWepMeshFirstP->GetSocketLocation(_MuzzleSocketName);
		FRotator rotation = SkCharWepMeshFirstP->GetSocketRotation(_MuzzleSocketName);
		_pLocalMuzzleEffect = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _pMuzzleEffectParticleSystem, position, rotation, FVector(1.0f, 1.0f, 1.0f));
	}
}

// Recoil *********************************************************************************************************************************

/*
*
*/
bool UFireMode::OwningClient_Reliable_RecoilCamera_Validate()
{ return true; }

void UFireMode::OwningClient_Reliable_RecoilCamera_Implementation()
{
	if (_fFiringTime < 1.0f) { return; }

	ABaseCharacter* character = Cast<ABaseCharacter>(_WeaponParentAttached->GetPawnOwner());
	FRotator currentRot = character->GetControlRotation();
	FRotator targetRot = currentRot;
	float pitch, yaw = 0.0f;

	// Random pitch (vertical) recoil pattern
	if (_bRandomPitchRecoil) {

		if (character->IsAiming()) 
		{ pitch = FMath::RandRange(_fRandomCameraAimingRecoilPitchMinimum, _fRandomCameraAimingRecoilPitchMaximum); }
		else 
		{ pitch = FMath::RandRange(_fRandomCameraHipfireRecoilPitchMinimum, _fRandomCameraHipfireRecoilPitchMaximum); }
	}

	// No random Pitch (vertical) recoil pattern
	else
	{ pitch = character->IsAiming() ? _fCameraAimingRecoilPitch : _fCameraHipfireRecoilPitch; }

	// Random Yaw (sideways) recoil pattern
	if (_bRandomYawRecoil) {

		if (character->IsAiming())
		{ yaw = FMath::RandRange(_fRandomCameraAimingRecoilYawMinimum, _fRandomCameraAimingRecoilYawMaximum); }
		else 
		{ yaw = FMath::RandRange(_fRandomCameraHipfireRecoilYawMinimum, _fRandomCameraHipfireRecoilYawMaximum); }
	}

	// No random Yaw (sideways) recoil pattern
	else
	{ yaw = character->IsAiming() ? _fCameraAimingRecoilYaw : _fCameraHipfireRecoilYaw; }

	targetRot += FRotator(pitch, yaw, 0.0f);
	targetRot.Roll = 0.0f; // Ensures that the controller doesn't somehow manage to affect "roll" in the FRotator...

	// Interpolate to the new FRotator
	FRotator newRot = currentRot;
	newRot = UKismetMathLibrary::RInterpTo_Constant(currentRot, targetRot, GetWorld()->GetDeltaSeconds(), _fCurrentRecoilInterpolationSpeed);

	///GEngine->AddOnScreenDebugMessage(6, 5.0f, FColor::Purple, TEXT("Roll: ") + FString::SanitizeFloat(newRot.Roll));
	///GEngine->AddOnScreenDebugMessage(7, 5.0f, FColor::Purple, TEXT("Pitch: ") + FString::SanitizeFloat(newRot.Pitch));
	///GEngine->AddOnScreenDebugMessage(8, 5.0f, FColor::Purple, TEXT("Yaw: ") + FString::SanitizeFloat(newRot.Yaw));

	// Add recoil to the controller
	AController* controller = character->GetController();
	if (controller == NULL) { return; }
	APlayerController* playerController = Cast<APlayerController>(controller);
	if (playerController != NULL)
	{
		playerController->SetControlRotation(newRot);
	}
}

///////////////////////////////////////////////

/*
*
*/
bool UFireMode::OwningClient_Reliable_StartRecoilInterpolation_Validate()
{ return true; }

void UFireMode::OwningClient_Reliable_StartRecoilInterpolation_Implementation()
{
	// Restart from zero (safety measure)
	_fFiringTime = 1.0f;
	_bUpdateRecoilInterpolation = true;
}

///////////////////////////////////////////////

/*
*
*/
bool UFireMode::OwningClient_Reliable_StopRecoilInterpolation_Validate()
{ return true; }

void UFireMode::OwningClient_Reliable_StopRecoilInterpolation_Implementation()
{
	// Firing time is finished - zero out
	_fFiringTime = 0.0f;
	_bUpdateRecoilInterpolation = false;
}

///////////////////////////////////////////////

void UFireMode::RecoilInterpolationUpdate()
{
	// Add to firing time
	_fFiringTime += GetWorld()->GetDeltaSeconds();

	float lerp = 0.0f;
	float alpha = UKismetMathLibrary::FClamp(_fFiringTime / _fRecoilInterpolationTransitionSpeed, 0.0f, 1.0f);

	// Aiming
	if (_WeaponParentAttached->GetIsAiming())
	{ lerp = UKismetMathLibrary::Lerp(_fRecoilInterpolationSpeedAimingMinimum, _fRecoilInterpolationSpeedAimingMaximum, alpha); }

	// Hipfire
	else
	{ lerp = UKismetMathLibrary::Lerp(_fRecoilInterpolationSpeedHipfireMinimum, _fRecoilInterpolationSpeedHipfireMaximum, alpha); }

	_fCurrentRecoilInterpolationSpeed = lerp;
}

// Reload *********************************************************************************************************************************

/*
*
*/
bool UFireMode::Server_Reliable_SetReloadComplete_Validate(bool ReloadComplete)
{ return true; }

void UFireMode::Server_Reliable_SetReloadComplete_Implementation(bool ReloadComplete)
{_bReloadComplete = ReloadComplete; }

///////////////////////////////////////////////

/*
*
*/
bool UFireMode::Server_Reliable_EjectMagazine_Validate()
{ return true; }

void UFireMode::Server_Reliable_EjectMagazine_Implementation()
{
	// Store reload gap amount (amount of shots fired from a maximum magazine)
	if (_pAmmoPool == NULL) { DetermineAmmoPool(); return; }
	_pAmmoPool->Server_Reliable_SetShotsFiredBeforeReload(_pAmmoPool->GetMaximumMagazineAmmo() - _pAmmoPool->GetMagazineAmmo());

	// Set magazine to zero
	///_bIsRoundInChamber = _iMagazineAmmoCount > 0;
	_bIsMagazineInWeapon = false;
	Server_Reliable_SetMagazineInWeapon(false);
	_pAmmoPool->Server_Reliable_SetMagazineCount(0);

	// Set reload stage
	_eReloadStage = E_ReloadStage::eRS_NewMagazine;
}

///////////////////////////////////////////////

/*
*
*/
bool UFireMode::Server_Reliable_NewMagazine_Validate()
{ return true; }

void UFireMode::Server_Reliable_NewMagazine_Implementation()
{
	if (_pAmmoPool == NULL) { DetermineAmmoPool(); return; }
	_pAmmoPool->Server_Reliable_Reload();

	// Set reload stage
	if (_bIsRoundInChamber) { _eReloadStage = E_ReloadStage::eRS_Ready; }
	else { _eReloadStage = E_ReloadStage::eRS_ChamberRound; }
}

///////////////////////////////////////////////

/*
*
*/
bool UFireMode::Server_Reliable_ChamberRound_Validate()
{ return true; }

void UFireMode::Server_Reliable_ChamberRound_Implementation()
{
	// Update bullet in chamber
	if (_pAmmoPool == NULL) { DetermineAmmoPool(); return; }
	_pAmmoPool->Server_Reliable_DetermineIfBulletShouldBeInChamber();

	// Set reload stage
	_eReloadStage = E_ReloadStage::eRS_Ready;
}

///////////////////////////////////////////////

bool UFireMode::Server_Reliable_SetReloadStage_Validate(E_ReloadStage ReloadState)
{ return true; }

void UFireMode::Server_Reliable_SetReloadStage_Implementation(E_ReloadStage ReloadStage)
{
	_eReloadStage = ReloadStage;
}

///////////////////////////////////////////////

float UFireMode::GetReloadStartingTime()
{
	float time = 0.0f;

	switch (_eReloadStage)
	{
	case E_ReloadStage::eRS_Ready: time = 0.0f; break;
	case E_ReloadStage::eRS_EjectMagazine: time = _fStartingTimeEjectMagazine; break;
	case E_ReloadStage::eRS_NewMagazine: time = _fStartingTimeNewMagazine; break;
	case E_ReloadStage::eRS_ChamberRound: time = _fStartingTimeChamberRound; break;

	default: break;
	}

	///FString msg = TEXT("Reload stage: " + Cast<ABaseCharacter>(_WeaponParentAttached->GetPawnOwner())->EnumToString(TEXT("E_ReloadStage"), static_cast<uint8>(_eReloadStage)));
	///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, msg);

	return time;
}

// Spread *********************************************************************************************************************************

/*
*
*/
float UFireMode::IncreaseSpread()
{
	bool instigatorAiming = _WeaponParentAttached->GetIsAiming();
	float clampMin = instigatorAiming ? _fSpreadAimingMinimum : _fSpreadHipMinimum;
	float clampMax = instigatorAiming ? _fSpreadAimingMaximum : _fSpreadHipMaximum;
	float additive = instigatorAiming ? _fSpreadAimingIncrease : _fSpreadHipIncrease;

	// Clamp
	_fProjectileSpread = FMath::Clamp(_fProjectileSpread, clampMin, clampMax);

	// Add spread
	Server_Reliable_AddToSpread(additive, clampMin, clampMax);
	if (_bIsUpdatingSpread) { _bCanDeductSpread = true; }

	return _fProjectileSpread;
}

///////////////////////////////////////////////

void UFireMode::UpdateSpreadToMinimum(bool Aiming)
{
	// If the current projectile spread is less than the minimum specified, then update the current spread to match the minimum value
	float minimumSpread = Aiming ? _fSpreadAimingMinimum : _fSpreadHipMinimum;
	if (GetCurrentProjectileSpread() < minimumSpread)
	{ _fProjectileSpread = minimumSpread; }
}

///////////////////////////////////////////////

/**
* @summary:	Sets the current projectile spread of the firemode.
*
* @networking:	Runs on server
*
* @param:	float Additive
* @param:	float ClampMaximum
*
* @return:	void
*/
bool UFireMode::Server_Reliable_AddToSpread_Validate(float Additive, float ClampMinimum, float ClampMaximum)
{ return true; }

void UFireMode::Server_Reliable_AddToSpread_Implementation(float Additive, float ClampMinimum, float ClampMaximum)
{
	_fProjectileSpread += Additive;
	_fProjectileSpread = FMath::Clamp(_fProjectileSpread, ClampMinimum, ClampMaximum);
}

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
bool UFireMode::Server_Reliable_DeductFromSpread_Validate(float Subtractive, float ClampMinimum, float ClampMaximum)
{ return true; }

void UFireMode::Server_Reliable_DeductFromSpread_Implementation(float Subtractive, float ClampMinimum, float ClampMaximum)
{
	_fProjectileSpread -= Subtractive;
	_fProjectileSpread = FMath::Clamp(_fProjectileSpread, ClampMinimum, ClampMaximum);
}
