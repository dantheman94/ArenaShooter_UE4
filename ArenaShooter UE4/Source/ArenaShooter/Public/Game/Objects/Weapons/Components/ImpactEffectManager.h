// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ImpactEffectManager.generated.h"

class UPhysicalMaterial;

// *** STRUCTS

USTRUCT(BlueprintType)
struct FImpactGroup
{
	GENERATED_BODY()

		FImpactGroup() {}

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UPhysicalMaterial* _mPhysicsMaterial = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UParticleSystem* _pImpactParticleSystem = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UMaterialInstance* _mImpactDecal = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<UMaterialInstance*> _mImpactDecals;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USoundBase* _sImpactSound = NULL;

	UPhysicalMaterial* GetPhysicsMaterial() { return _mPhysicsMaterial; }

	UParticleSystem* GetImpactParticleSystem() { return _pImpactParticleSystem; }

	UMaterialInstance* GetDecal() { return _mImpactDecal; }

	TArray<UMaterialInstance*> GetDecals() { return _mImpactDecals; }

	USoundBase* GetSound() { return _sImpactSound; }

};

// *** CLASSES

UCLASS()
class ARENASHOOTER_API AImpactEffectManager : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	AImpactEffectManager();

protected:

	// Startup **************************************************************

	// *** FUNCTIONS

	/**
	* @summary:	Called when the game starts or when spawned.
	*
	* @return:	virtual void
	*/
	virtual void BeginPlay() override;

	// *** VARIABLES

	// Properties ***********************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties")
		FRotator _rRandomDecalRotation = FRotator::ZeroRotator;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties")
		float _fDecalSize = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Properties")
		float _fDecalLifespan = 10.0f;

	// Impact Groups ********************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Impact Groups")
		TArray<FImpactGroup> _ImpactEffectGroups;

public:	

	// *** FUNCTIONS

	// Current Frame ********************************************************

	/**
	* @summary:	Called every frame.
	*
	* @param:	float DeltaTime
	*
	* @return:	virtual void
	*/
	virtual void Tick(float DeltaTime) override;

	// Spawning *************************************************************

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SpawnImpactEffect(FHitResult HitResult);

	/*
	*
	*/
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
		void Server_Unreliable_SpawnImpactEffectGroup(int ArrayPosition, FHitResult HitResult);

};
