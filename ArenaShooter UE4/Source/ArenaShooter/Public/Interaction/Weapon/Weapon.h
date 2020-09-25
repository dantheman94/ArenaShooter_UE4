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
	eHA_EquipDuelLeft UMETA(DisplayName = "Equip Duelwielding Left"),
	eHA_UnequipDuelLeft UMETA(DisplayName = "Unequip Duelwielding Left"),
	eHA_EquipDuelRight UMETA(DisplayName = "Equip Duelwielding Right"),
	eHA_UnequipDuelRight UMETA(DisplayName = "Unequip Duelwielding Right"),
	eHA_Inspect UMETA(DisplayName = "Inspect"),
	eHA_ReloadEjectMag UMETA(DisplayName = "Reload Eject Magazine"),
	eHA_ReloadNewMag UMETA(DisplayName = "Reload New Magazine"),
	eHA_ReloadChamberRound UMETA(DisplayName = "Reload Chamber Round"),
	eHA_ReloadFullNotEmpty UMETA(DisplayName = "Full Reload Not Empty"),
	eHA_ReloadFullEmpty UMETA(DisplayName = "Full Reload Empty"),
	eHA_ReloadDuelRightLower UMETA(DisplayName = "Reload Duel Right Lower"),
	eHA_ReloadDuelRightRaise UMETA(DisplayName = "Reload Duel Right Raise"),
	eHA_ReloadDuelLeftLower  UMETA(DisplayName = "Reload Duel Left Lower"),
	eHA_ReloadDuelLeftRaise  UMETA(DisplayName = "Reload Duel Left Raise"),
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
	eGA_ReloadDuelRightLower UMETA(DisplayName = "Reload Duel Right Lower"),
	eGA_ReloadDuelRightRaise UMETA(DisplayName = "Reload Duel Right Raise"),
	eGA_ReloadDuelLeftLower  UMETA(DisplayName = "Reload Duel Left Lower"),
	eGA_ReloadDuelLeftRaise  UMETA(DisplayName = "Reload Duel Left Raise"),
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

#pragma region Protected Variables

protected:

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Startup

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @summary:	Called when the game starts or when spawned.
	*
	* @return:	virtual void
	*/
	virtual void BeginPlay() override;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Current Frame 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Animation

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		FTransform _tOriginArms;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		FTransform _tOriginWeapon;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		bool _bSwapHandsOnFirstPickup = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		TSubclassOf<UAnimInstance> _AnimationBPFirstPersonArmsStandard = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation",
		meta = (EditCondition = "_bCanBeDuelWielded"))
		TSubclassOf<UAnimInstance> _AnimationBPFirstPersonArmsDuelWielding = NULL;
	
	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		TSubclassOf<UAnimInstance> _AnimationBPFirstPersonWeapon = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
		TSubclassOf<UAnimInstance> _AnimationBPThirdPersonCharacter = NULL;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Attachments | Scope

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Attachments | Flashlight

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Duel Wielding

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		float _fDuelWieldingReloadTime = 1.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Duel Wielding",
		meta = (EditCondition = "_bCanBeDuelWielded"))
		FTransform _tDuelOriginPrimaryArms;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Duel Wielding",
		meta = (EditCondition = "_bCanBeDuelWielded"))
		FTransform _tDuelOriginPrimaryWeapon;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Duel Wielding",
		meta = (EditCondition = "_bCanBeDuelWielded"))
		FTransform _tDuelOriginSecondaryArms;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Duel Wielding",
		meta = (EditCondition = "_bCanBeDuelWielded"))
		FTransform _tDuelOriginSecondaryWeapon;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Interaction

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Interaction")
		TSubclassOf<AActor> _OnDroppedActor = NULL;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Properties

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

#pragma endregion Protected Variables

#pragma region Public Functions

public:

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Startup

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Sets default values for this actor's properties
	AWeapon();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

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
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetPawnOwnerIsAiming(bool Aiming);

	UFUNCTION(BlueprintPure)
		bool GetIsAiming() const { return _bIsPawnOwnerAiming; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetFireModeIterator() { return _iFiringModeIterator; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		TArray<UFireMode*> GetFireModes() { return _uFiringModes; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		UFireMode* GetCurrentFireMode() const { return _uFiringModes[_iFiringModeIterator]; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		bool IsThereValidFireModeCount() const { return _uFiringModes.Num() >= 1 && _uFiringModes.Num() >= _iFiringModeIterator; }

	/*
	*
	*/
	UFUNCTION()
		void SetOwnersPrimaryWeapon(bool IsPrimaryWeapon) { _bIsPawnOwnersPrimaryWeapon = IsPrimaryWeapon; }

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetOwnersPrimaryWeapon(bool IsPrimaryWeapon);

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		bool IsOwnersPrimaryWeapon() { return _bIsPawnOwnersPrimaryWeapon; }

	/*
	*
	*/
	UFUNCTION()
		void SetOwnersSecondaryWeapon(bool IsSecondaryWeapon) { _bIsPawnOwnersSecondaryWeapon = IsSecondaryWeapon; }

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetOwnersSecondaryWeapon(bool IsSecondaryWeapon);

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		bool IsOwnersSecondaryWeapon() { return _bIsPawnOwnersSecondaryWeapon; }

	/*
	*
	*/
	UFUNCTION()
		void SetNewOwner(APawn* NewOwner) { _PawnOwner = NewOwner; }

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetNewOwner(APawn* NewOwner);

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		APawn* GetPawnOwner() { return _PawnOwner; }

	/*
	*
	*/
	UFUNCTION()
		TArray<UAmmo*> GetAmmoPool() { return _pAmmoPools; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		bool CanBeDuelWielded() const { return _bCanBeDuelWielded; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Animation 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		FTransform GetTransformOriginArms() const { return _tOriginArms; }

	/*
	*
	*/
	UFUNCTION()
		FTransform GetTransformOriginWeapon() const { return _tOriginWeapon; }

	/*
	*
	*/
	UFUNCTION()
		TSubclassOf<UAnimInstance> GetAnimInstanceFirstPersonHands() { return _AnimationBPFirstPersonArmsStandard; }

	/*
	*
	*/
	UFUNCTION()
		TSubclassOf<UAnimInstance> GetAnimInstanceFirstPersonGun() { return _AnimationBPFirstPersonWeapon; }

	/*
	*
	*/
	UFUNCTION()
		TSubclassOf<UAnimInstance> GetAnimInstanceThirdPersonCharacter() { return _AnimationBPThirdPersonCharacter; }

	/**
	* @summary:	Returns reference to an animation montage used on the weapon owner's first person mesh.
	*
	* @param:	E_HandAnimation AnimationEnum
	*
	* @return:	UAnimMontage*
	*/
	UFUNCTION()
		UAnimMontage* GetArmAnimation(E_HandAnimation AnimationEnum);

	/**
	* @summary:	Returns reference to an animation montage used on the weapon's mesh.
	*
	* @param:	E_GunAnimation AnimationEnum
	*
	* @return:	UAnimMontage*
	*/
	UFUNCTION()
		UAnimMontage* GetGunAnimation(E_GunAnimation AnimationEnum);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Properties

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		USkeletalMesh* GetFirstPersonMesh() const { return _FirstPersonMesh; }

	/*
	*
	*/
	UFUNCTION()
		USkeletalMesh* GetThirdPersonMesh() const { return _ThirdPersonMesh; }

	/*
	*
	*/
	UFUNCTION()
		TSubclassOf<AActor> GetOnDroppedActor() { return _OnDroppedActor; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		FTransform GetOriginHands() const { return _tOriginArms; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Attachments | Scope

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		bool IsScopeAttachmentEnabled() const { return _bScopeEnabled; }

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		UStaticMesh* GetSightMesh() { return _uScopeMesh; }

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		FName GetSightAttachmentName() const { return _ScopeAttachmentSocket; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Attachments | Flashlight

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		FName GetFlashlightAttachmentName() const { return _FlashlightAttachmentSocket; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Duel Wielding

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		float GetDuelWieldingReloadTime() const { return _fDuelWieldingReloadTime; }

	/*
	*
	*/
	UFUNCTION()
		FTransform GetTransformOriginDuelPrimaryArms() const { return _tDuelOriginPrimaryArms; }

	/*
	*
	*/
	UFUNCTION()
		FTransform GetTransformOriginDuelSecondaryArms() const { return _tDuelOriginSecondaryArms; }

	/*
	*
	*/
	UFUNCTION()
		FTransform GetTransformOriginDuelPrimaryWeapon() const { return _tDuelOriginPrimaryWeapon; }

	/*
	*
	*/
	UFUNCTION()
		FTransform GetTransformOriginDuelSecondaryWeapon() const { return _tDuelOriginSecondaryWeapon; }

#pragma endregion Public Functions

};
