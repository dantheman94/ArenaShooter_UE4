// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

#include "Ammo.h"
#include "BaseCharacter.h"
#include "FireMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnrealNetwork.h"

/**
* @summary:	Sets default values for this actor's properties.
*
* @return:	Constructor
*/
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Actor replicates
	bReplicates = true;
	SetReplicateMovement(true);
	SetRemoteRoleForBackwardsCompat(ROLE_AutonomousProxy);

	///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("Constructor Called"));

	// Get all firing mode components as reference
	///this->GetComponents<UFireMode>(_uFiringModes);
	
	///FString _Message = TEXT("Firemode iter: ") + FString::FromIntm(this->GetFireModeIterator()) +
	///	TEXT(" / firemode size: ") + FString::FromInt(this->_uFiringModes.Num());
	///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, _Message);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, _bIsPawnOwnerAiming);
	DOREPLIFETIME(AWeapon, _PawnOwner);
}

/**
* @summary:	Called when the game starts or when spawned.
*
* @return:	virtual void
*/
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Get reference to the interactable weapon we should be attached to already
	AInteractableWeapon* interactableWeapon = Cast<AInteractableWeapon>(GetOwner());
	if (interactableWeapon == NULL)
		return;
	interactableWeapon->InitializeFromAbstract(this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Current Frame

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* @summary:	Called every frame.
*
* @param:	DeltaTime The delta of the frame.
*
* @return:	virtual void
*/
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AWeapon::Server_Reliable_SetNewOwner_Validate(APawn* NewOwner) { return true; }
void AWeapon::Server_Reliable_SetNewOwner_Implementation(APawn* NewOwner) { SetNewOwner(NewOwner); }
void AWeapon::SetNewOwner(APawn* NewOwner)
{
	if (GetLocalRole() == ROLE_Authority)
		_PawnOwner = NewOwner;
	else
		Server_Reliable_SetNewOwner(NewOwner);
}

bool AWeapon::Server_Reliable_SetOwnersPrimaryWeapon_Validate(bool IsPrimaryWeapon) { return true; }
void AWeapon::Server_Reliable_SetOwnersPrimaryWeapon_Implementation(bool IsPrimaryWeapon)
{
	SetOwnersPrimaryWeapon(IsPrimaryWeapon);

	// If this is the owners primary weapon, then it cant be their secondary weapon too
	if (IsPrimaryWeapon) { _bIsPawnOwnersSecondaryWeapon = false; }

	// Create UMG crosshair widgets
	///for (int i = 0; i < _uFiringModes.Num(); ++i)
	///{
	///	_uFiringModes[i]->Client_Reliable_CreateAndAssignCrosshair(NULL);
	///}
}
void AWeapon::SetOwnersPrimaryWeapon(bool IsPrimaryWeapon)
{
	if (GetLocalRole() == ROLE_Authority)
		_bIsPawnOwnersPrimaryWeapon = IsPrimaryWeapon;
	else
		Server_Reliable_SetOwnersPrimaryWeapon(IsPrimaryWeapon);
}

bool AWeapon::Server_Reliable_SetOwnersSecondaryWeapon_Validate(bool IsSecondaryWeapon) { return true; }
void AWeapon::Server_Reliable_SetOwnersSecondaryWeapon_Implementation(bool IsSecondaryWeapon)
{
	SetOwnersSecondaryWeapon(IsSecondaryWeapon);

	// If this is the owners secondary weapon, then it cant be their primary weapon too
	if (IsSecondaryWeapon) { SetOwnersPrimaryWeapon(false); }

	// Create UMG crosshair widgets
	///for (int i = 0; i < _uFiringModes.Num(); ++i)
	///{
	///	_uFiringModes[i]->Client_Reliable_CreateAndAssignCrosshair(NULL);
	///}
}
void AWeapon::SetOwnersSecondaryWeapon(bool IsSecondaryWeapon)
{
	if (GetLocalRole() == ROLE_Authority)
		_bIsPawnOwnersSecondaryWeapon = IsSecondaryWeapon;
	else
		Server_Reliable_SetOwnersSecondaryWeapon(IsSecondaryWeapon);
}

bool AWeapon::Server_Reliable_SetPawnOwnerIsAiming_Validate(bool Aiming) { return true; }
void AWeapon::Server_Reliable_SetPawnOwnerIsAiming_Implementation(bool Aiming)
{ 
	_bIsPawnOwnerAiming = Aiming; 

	// Updating spread minimum if required
	UFireMode* fireMode = GetCurrentFireMode();
	if (fireMode != NULL)
	{ fireMode->UpdateSpreadToMinimum(Aiming); }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Animation

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* @summary:	Returns reference to an animation montage used on the weapon owner's first person mesh.
*
* @param:	E_HandAnimation AnimationEnum
*
* @return:	UAnimMontage*
*/
UAnimMontage* AWeapon::GetArmAnimation(E_HandAnimation AnimationEnum)
{
	UAnimMontage* montage = NULL;

	// Sanity check
	if (_uFiringModes.Num() >= _iFiringModeIterator)
	{
		// Get the animation from the current fire mode
		UFireMode* fireMode = _uFiringModes[_iFiringModeIterator];
		if (fireMode != NULL) { montage = fireMode->GetArmAnimation(AnimationEnum); }
	}

	return montage;
}

/**
* @summary:	Returns reference to an animation montage used on the weapon's mesh.
*
* @param:	E_GunAnimation AnimationEnum
*
* @return:	UAnimMontage*
*/
UAnimMontage* AWeapon::GetGunAnimation(E_GunAnimation AnimationEnum)
{
	UAnimMontage* montage = NULL;

	// Sanity check
	if (_uFiringModes.Num() >= _iFiringModeIterator)
	{
		// Get the animation from the current fire mode
		UFireMode* fireMode = _uFiringModes[_iFiringModeIterator];
		if (fireMode != NULL) { montage = fireMode->GetGunAnimation(AnimationEnum); }
	}

	return montage;
}
