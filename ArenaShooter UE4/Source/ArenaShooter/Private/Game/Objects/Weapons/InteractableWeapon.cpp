// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableWeapon.h"

#include "Ammo.h"
#include "FireMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnrealNetwork.h"
#include "UObject/UObjectBaseUtility.h"
#include "Weapon.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Startup 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*	Sets default values for this actor's properties.
*/
AInteractableWeapon::AInteractableWeapon()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Actor replicates
	bReplicates = true;
	SetReplicateMovement(true);

	// Create components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	_InteractionTrigger = CreateDefaultSubobject<UInteractableSphereComponent>(("Focus Trigger"));
	_InteractionTrigger->SetupAttachment(RootComponent);
	_WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh"));
	_WeaponMesh->SetupAttachment(_InteractionTrigger);
	_InteractionData = CreateDefaultSubobject<UInteractionDataComponent>(TEXT("Interaction Data"));
}

void AInteractableWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractableWeapon, _iMagazineSize);
	DOREPLIFETIME(AInteractableWeapon, _iReserveSize);
	DOREPLIFETIME(AInteractableWeapon, _iBatterySize);
	DOREPLIFETIME(AInteractableWeapon, _AbstractWeapon);
}

/*
*	Called when the game starts or when spawned.
*/
void AInteractableWeapon::BeginPlay()
{
	Super::BeginPlay();	

	if (_InteractionData == NULL)
		return;
	
	// Spawn abstract weapon instance from class
	if (_AbstractWeapon == NULL)
	{
		// Spawn info
		FVector location(0.0f, 0.0f, 0.0f);
		FRotator rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters spawnInfo;
		spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Spawn weapon
		AActor* spawnedActor = GetWorld()->SpawnActor<AActor>(_InteractionData->GetOnUsedActor(), location, rotation, spawnInfo);
		AWeapon* _AbstractWeapon = Cast<AWeapon>(spawnedActor);

#if WITH_EDITOR
		_AbstractWeapon->SetFolderPath_Recursively(FName(TEXT("Runtime Spawned Actors/Abstract/Weapons/" + UKismetSystemLibrary::GetClassDisplayName(UGameplayStatics::GetObjectClass(_AbstractWeapon)))));
#endif
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Ammo 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*
*/
bool AInteractableWeapon::Server_Reliable_SetMagazineSize_Validate(int size) { return true; }
void AInteractableWeapon::Server_Reliable_SetMagazineSize_Implementation(int size) { SetMagazineSize(size); }
void AInteractableWeapon::SetMagazineSize(int size)
{
	if (GetLocalRole() == ROLE_Authority) 
		_iMagazineSize = size;	
	else
		Server_Reliable_SetMagazineSize(size);
}

/*
*
*/
bool AInteractableWeapon::Server_Reliable_SetReserveSize_Validate(int size) { return true; }
void AInteractableWeapon::Server_Reliable_SetReserveSize_Implementation(int size) { SetReserveSize(size); }
void AInteractableWeapon::SetReserveSize(int size)
{
	if (GetLocalRole() == ROLE_Authority)
		_iReserveSize = size;
	else
		Server_Reliable_SetReserveSize(size);
}

/*
*
*/
bool AInteractableWeapon::Server_Reliable_SetBatterySize_Validate(int size) { return true; }
void AInteractableWeapon::Server_Reliable_SetBatterySize_Implementation(int size) { SetBatterySize(size);  }
void AInteractableWeapon::SetBatterySize(int size)
{
	if (GetLocalRole() == ROLE_Authority)
		_iBatterySize = size;
	else
		Server_Reliable_SetBatterySize(size);
}

/*
*
*/
void AInteractableWeapon::InitializeFromAbstract(AWeapon* AbstractWeapon)
{
	if (_AbstractWeapon != AbstractWeapon)	
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Error: Something unexpected happened.. Missmatch between AInteractableWeapon::_AbstractWeapon && AbstractWeapon reference."));
	else
	{
		// Add ammo components according to the abstract "OnUsedActorClass" (this is a weapon)
		TArray<UAmmo*> abstractAmmoComponents = _AbstractWeapon->GetAmmoPool();
		for (int i = 0; i < _AbstractWeapon->GetAmmoPool().Num(); i++)
		{
			UAmmo* abstractAmmoPool = Cast<UAmmo>(_AbstractWeapon->GetAmmoPool()[i]);

			// Add component
			UAmmo* a = NewObject<UAmmo>(this->GetOwner(), UAmmo::StaticClass(), FName("_Ammo_" + i));
			a->RegisterComponent();

			// File component with data
			a->SetMagazineCount(abstractAmmoPool->GetStartingMagazineAmmo());
			a->SetReserveCount(abstractAmmoPool->GetStartingReserveAmmo());
			a->SetBatteryAmmo(abstractAmmoPool->GetStartingBatteryAmmo());
		}

		// Set starting ammo amounts in the interactable weapon class, based off the settings in the abstract class
		TSubclassOf<AWeapon> weaponClass = StaticCast<TSubclassOf<AWeapon>>(_InteractionData->GetOnUsedActor());
		if (weaponClass)
		{
			// Need to get the class defaults by retrieving the static object instance of the actor class
			AWeapon* staticWeapon = weaponClass.GetDefaultObject();
			if (staticWeapon == NULL)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("STATIC CLASS CAST ERROR"));
				return;
			}

			// Set default ammo values (NEED TO ACCOUNT FOR ALL AMMO POOLS IN THE FUTURE)
			// TO DO: 
			// 1) Confirm that interactable/abstract class have matching ammo pool components
			// 2) set interactable ammo sizes based on the abstract class (FOR ALL AMMO POOLS)
			///SetMagazineSize(staticWeapon->GetCurrentFireMode()->GetAmmoPool()->GetStartingMagazineAmmo());
			///SetReserveSize(staticWeapon->GetCurrentFireMode()->GetAmmoPool()->GetStartingReserveAmmo());
			///SetBatterySize(staticWeapon->GetCurrentFireMode()->GetAmmoPool()->GetStartingReserveAmmo());
		}
	}
}
