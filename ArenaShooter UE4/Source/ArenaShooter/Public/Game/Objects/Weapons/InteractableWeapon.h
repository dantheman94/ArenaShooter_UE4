// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "InteractableSphereComponent.h"

#include "InteractableWeapon.generated.h"

// *** DEFINITIONS

#define _MAX_MAGAZINE_AMMO	100
#define _MAX_RESERVE_AMMO	100

// *** CLASSES

class AWeapon;

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API AInteractableWeapon : public AActor
{
	GENERATED_BODY()

#pragma region Protected Variables

protected:

	//
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		UStaticMeshComponent* _WeaponMesh = NULL;

	//
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		UInteractionDataComponent* _InteractionData = NULL;

	//
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		UInteractableSphereComponent* _InteractionTrigger = NULL;

	//
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Replicated)
		int _iMagazineSize = _MAX_MAGAZINE_AMMO;

	//
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Replicated)
		int _iReserveSize = _MAX_RESERVE_AMMO;

	//
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Replicated)
		int _iBatterySize = _MAX_MAGAZINE_AMMO;

	//
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Replicated)
		AWeapon* _AbstractWeapon = NULL;

#pragma endregion Protected Variables

#pragma region Public Functions

public:

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Startup 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*	Sets default values for this actor's properties.
	*/
	AInteractableWeapon();

	/*
	*
	*/
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

	/*
	*	Called when the game starts or when spawned.
	*/
	virtual void BeginPlay() override;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Ammo 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_SetMagazineSize(int size);
	void SetMagazineSize(int size);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_SetReserveSize(int size);
	void SetReserveSize(int size);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_SetBatterySize(int size);
	void SetBatterySize(int size);

	void InitializeFromAbstract(AWeapon* AbstractWeapon);

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Ammo") float GetMagazineSize() { return _iMagazineSize; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Ammo") float GetReserveSize() { return _iReserveSize; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Ammo") float GetBatterySize() { return _iBatterySize; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Interaction 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure, Category = "Interaction") UInteractionDataComponent* GetInteractionDataComponent() { return _InteractionData; }

#pragma endregion Public Functions

};
