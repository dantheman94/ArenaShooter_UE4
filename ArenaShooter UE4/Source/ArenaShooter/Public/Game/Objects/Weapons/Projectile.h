// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

// *** CLASSES

class AWeapon;
class UProjectileMovementComponent;
class USphereComponent;
class uStaticMesh;

UCLASS()
class ARENASHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Startup ********************************************************************************************************************************
	AProjectile();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UProjectileMovementComponent* _uProjectileMovement;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USphereComponent* _SphereCollision;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UStaticMeshComponent* _StaticMesh;

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

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties")
		AWeapon* _Weapon = NULL;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties")
		FVector _vProjectileStartingLocation = FVector::ZeroVector;

	// Bleedthrough ***************************************************************************************************************************

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Bleedthrough")
		bool _bBleedthroughEnabled = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Bleedthrough",
		meta = (EditCondition = "_bBleedthroughEnabled"))
		float _fBleedthroughMultiplier = 1.0f;

	// Speed **********************************************************************************************************************************

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Speed")
		float _fInitialSpeed = 4000.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Speed")
		float _fMaximumSpeed = 6000.0f;

	// Tracking *******************************************************************************************************************************

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Tracking")
		bool _bProjectileTracksTargets = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Tracking",
		meta = (EditCondition = "_bProjectileTracksTargets"))
		float _fTrackingStrength = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Tracking",
		meta = (EditCondition = "_bProjectileTracksTargets"))
		float _fStopTrackingDistanceThreshold = 10.0f;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Tracking")
		AActor* _ProjectileTarget = NULL;

public:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Startup ********************************************************************************************************************************

	virtual void Init(AActor* ProjectileTrackingTarget = NULL);

	///////////////////////////////////////////////

	UFUNCTION()
		void SetWeapon(AWeapon* Weapon) { _Weapon = Weapon; }

	// Current Frame **************************************************************************************************************************

	/**
	* @summary:	Called every frame.
	*
	* @param:	float DeltaTime
	*
	* @return:	virtual void
	*/
	virtual void Tick(float DeltaTime) override;

	// Impact *********************************************************************************************************************************

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult);

	///////////////////////////////////////////////

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult);

};
