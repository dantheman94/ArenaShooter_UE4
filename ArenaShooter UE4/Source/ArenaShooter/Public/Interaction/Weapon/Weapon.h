// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Core/Public/Math/Transform.h"
#include "Runtime/Engine/Classes/Curves/CurveFloat.h"
#include "Structures.h"
#include "Weapon.generated.h"

// *** ENUMERATORS

UENUM(BlueprintType)
enum class E_HandAnimation : uint8
{
	eHA_FirstPickup UMETA(DisplayName = "First Pickup"),
	eHA_Equip UMETA(DisplayName = "Equip"),
	eHA_Unequip UMETA(DisplayName = "Unequip"),
	eHA_Inspect UMETA(DisplayName = "Inspect"),
	eHA_ReloadEjectMag UMETA(DisplayName = "Reload Eject Magazine"),
	eHA_ReloadNewMag UMETA(DisplayName = "Reload New Magazine"),
	eHA_ReloadChamberRound UMETA(DisplayName = "Reload Chamber Round"),
	eHA_ReloadFullNotEmpty UMETA(DisplayName = "Full Reload Not Empty"),
	eHA_ReloadFullEmpty UMETA(DisplayName = "Full Reload Empty"),
	eHA_ReloadDuelRight UMETA(DisplayName = "Reload Duel Right"),
	eHA_ReloadDuelLeft  UMETA(DisplayName = "Reload Duel Left"),
	eHA_FireProjectileHipfire UMETA(DisplayName = "Fire Projectile Hipfire"),
	eHA_FireProjectileAiming UMETA(DisplayName = "Fire Projectile Aiming"),
	eHA_ProjectileMisfire UMETA(DisplayName = "Projectile Misfire"),
	eHA_ProjectilePump UMETA(DisplayName = "Pump Projectile"),
	eHA_LoopingReloadEnter UMETA(DisplayName = "Looping Reload Enter"),
	eHA_LoopingReloadLoop UMETA(DisplayName = "Looping Reload Loop"),
	eHA_LoopingReloadExit UMETA(DisplayName = "Looping Reload Exit")
};

UENUM(BlueprintType)
enum class E_GunAnimation : uint8
{
	eGA_FirstPickup UMETA(DisplayName = "First Pickup"),
	eGA_Inspect UMETA(DisplayName = "Inspect"),
	eGA_ReloadEjectMag UMETA(DisplayName = "Reload Eject Magazine"),
	eGA_ReloadNewMag UMETA(DisplayName = "Reload New Magazine"),
	eGA_ReloadChamberRound UMETA(DisplayName = "Reload Chamber Round"),
	eGA_ReloadFullNotEmpty UMETA(DisplayName = "Full Reload Not Empty"),
	eGA_ReloadFullEmpty UMETA(DisplayName = "Full Reload Empty"),
	eGA_ReloadDuelRight UMETA(DisplayName = "Reload Duel Right"),
	eGA_ReloadDuelLeft  UMETA(DisplayName = "Reload Duel Left"),
	eGA_FireProjectileHipfire UMETA(DisplayName = "Fire Projectile Hipfire"),
	eGA_FireProjectileAiming UMETA(DisplayName = "Fire Projectile Aiming"),
	eGA_ProjectileMisfire UMETA(DisplayName = "Projectile Misfire"),
	eGA_ProjectilePump UMETA(DisplayName = "Pump Projectile"),
	eGA_LoopingReloadEnter UMETA(DisplayName = "Looping Reload Enter"),
	eGA_LoopingReloadLoop UMETA(DisplayName = "Looping Reload Loop"),
	eGA_LoopingReloadExit UMETA(DisplayName = "Looping Reload Exit")
};

// *** CLASSES

class APawn;
class USkeletalMesh;
class ABaseCharacter;
class UFireMode;
class UAmmo;

UCLASS(Blueprintable)
class ARENASHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:

	// Startup **************************************************************

	// Sets default values for this actor's properties
	AWeapon();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:

	// Startup ********************************************************************************************************************************

	// *** FUNCTIONS

	/**
	* @summary:	Called when the game starts or when spawned.
	*
	* @return:	virtual void
	*/
	virtual void BeginPlay() override;

	// *** VARIABLES

	// Current Frame **************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		bool _bIsPawnOwnersPrimaryWeapon = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		bool _bIsPawnOwnersSecondaryWeapon = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		bool _bIsPawnOwnerAiming = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		bool _bIsPawnOwnerFiring = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		bool _bHasPawnOwnerStoppedFiring = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		int _iFiringModeIterator = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Current")
		TArray<UFireMode*> _uFiringModes;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current", Replicated)
		APawn* _PawnOwner = NULL;

	// Animation ******************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		FTransform _tOriginHands;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		FTransform _tOriginGun;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		bool _bSwapHandsOnFirstPickup = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		TSubclassOf<UAnimInstance> _AnimationBPFirstPersonHands = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		TSubclassOf<UAnimInstance> _AnimationBPFirstPersonGun = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		TSubclassOf<UAnimInstance> _AnimationBPThirdPersonCharacter = NULL;

	// Attachments | Scope ********************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attachments | Scope")
		bool _bScopeEnabled = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attachments | Scope")
		UStaticMesh* _uScopeMesh = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attachments | Scope")
		FName _ScopeAttachmentSocket = TEXT("SightAttachmentPoint");

	// Attachments | Flashlight ***************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attachments | Flashlight")
		bool _bFlashlightEnabled = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attachments | Flashlight")
		UStaticMesh* _uFlashlightMesh = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attachments | Flashlight")
		FName _FlashlightAttachmentSocket = TEXT("FlashlightAttachmentPoint");

	// Duel Wielding **************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Duel Wielding")
		bool _bCanBeDuelWielded = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Duel Wielding",
		meta = (EditCondition = "_bCanBeDuelWielded"))
		FTransform _tDuelOriginPrimary;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Duel Wielding",
		meta = (EditCondition = "_bCanBeDuelWielded"))
		FTransform _tDuelOriginSecondary;

	// Interaction ****************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Interaction")
		TSubclassOf<AActor> _OnDroppedActor = NULL;

	// Properties *****************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Properties")
		USkeletalMesh* _FirstPersonMesh = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Properties")
		USkeletalMesh* _ThirdPersonMesh = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Properties")
		TArray<UAmmo*> _pAmmoPools;

public:

	// *** FUNCTIONS

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
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetPawnOwnerIsAiming(bool Aiming);

	///////////////////////////////////////////////

	UFUNCTION(BlueprintPure)
		bool GetIsAiming() { return _bIsPawnOwnerAiming; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetFireModeIterator() { return _iFiringModeIterator; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		TArray<UFireMode*> GetFireModes() { return _uFiringModes; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		UFireMode* GetCurrentFireMode() { return _uFiringModes[_iFiringModeIterator]; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		bool IsThereValidFireModeCount() { return _uFiringModes.Num() >= 1 && _uFiringModes.Num() >= _iFiringModeIterator; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetOwnersPrimaryWeapon(bool IsPrimaryWeapon);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetOwnersSecondaryWeapon(bool IsSecondaryWeapon);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		bool IsOwnersPrimaryWeapon() { return _bIsPawnOwnersPrimaryWeapon; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		bool IsOwnersSecondaryWeapon() { return _bIsPawnOwnersSecondaryWeapon; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		bool CanBeDuelWielded() { return _bCanBeDuelWielded; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetNewOwner(APawn* NewOwner);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		APawn* GetPawnOwner() { return _PawnOwner; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		TArray<UAmmo*> GetAmmoPool() { return _pAmmoPools; }

	// Animation ******************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		FTransform GetTransformOriginHands() { return _tOriginHands; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		FTransform GetTransformOriginGun() { return _tOriginGun; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		FTransform GetTransformOriginDuelPrimary() { return _tDuelOriginPrimary; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		FTransform GetTransformOriginDuelSecondary() { return _tDuelOriginSecondary; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		TSubclassOf<UAnimInstance> GetAnimInstanceFirstPersonHands() { return _AnimationBPFirstPersonHands; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		TSubclassOf<UAnimInstance> GetAnimInstanceFirstPersonGun() { return _AnimationBPFirstPersonGun; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		TSubclassOf<UAnimInstance> GetAnimInstanceThirdPersonCharacter() { return _AnimationBPThirdPersonCharacter; }

	///////////////////////////////////////////////

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

	// Properties *****************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		USkeletalMesh* GetFirstPersonMesh() { return _FirstPersonMesh; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		USkeletalMesh* GetThirdPersonMesh() { return _ThirdPersonMesh; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		TSubclassOf<AActor> GetOnDroppedActor() { return _OnDroppedActor; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		FTransform GetOriginHands() { return _tOriginHands; }

	// Attachments | Scope ********************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		bool IsScopeAttachmentEnabled() { return _bScopeEnabled; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		UStaticMesh* GetSightMesh() { return _uScopeMesh; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		FName GetSightAttachmentName() { return _ScopeAttachmentSocket; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		FName GetFlashlightAttachmentName() { return _FlashlightAttachmentSocket; }

};
