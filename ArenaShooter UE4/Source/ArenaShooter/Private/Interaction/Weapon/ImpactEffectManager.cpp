// Fill out your copyright notice in the Description page of Project Settings.

#include "ImpactEffectManager.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/RandomStream.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "UnrealNetwork.h"

// Startup ********************************************************************************************************************************

/**
* @summary:	Sets default values for this actor's properties.
*
* @return:	Constructor
*/
AImpactEffectManager::AImpactEffectManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Cannot be damaged
	SetCanBeDamaged(false);

	// Actor replicates
	bReplicates = true;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
}

///////////////////////////////////////////////

/**
* @summary:	Called when the game starts or when spawned.
*
* @return:	virtual void
*/
void AImpactEffectManager::BeginPlay()
{
	Super::BeginPlay();

}

// Current Frame **************************************************************************************************************************

/**
* @summary:	Called every frame.
*
* @param:	DeltaTime	- The delta of the frame.
*
* @return:	virtual void
*/
void AImpactEffectManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Spawning *******************************************************************************************************************************

/*
*
*/
bool AImpactEffectManager::Server_Reliable_SpawnImpactEffect_Validate(FHitResult HitResult)
{ return true; }

void AImpactEffectManager::Server_Reliable_SpawnImpactEffect_Implementation(FHitResult HitResult)
{	
	// Get physics material
	TWeakObjectPtr<UPhysicalMaterial> physicsMat = HitResult.PhysMaterial;
	EPhysicalSurface surfaceType;
	if (physicsMat == NULL) { return; }
	surfaceType = physicsMat->SurfaceType;
	if (surfaceType == NULL) {

		// Play default effect
		EPhysicalSurface defaultSurface = EPhysicalSurface::SurfaceType1;
		for (int i = 0; i < _ImpactEffectGroups.Num(); i++)
		{
			if (_ImpactEffectGroups[i].GetPhysicsMaterial()->SurfaceType == defaultSurface)
			{
				// Play the effect on all clients
				Server_Unreliable_SpawnImpactEffectGroup(i, HitResult);
				break;
			}
		}
		return;
	}

	// Find matching effect group
	for (int i = 0; i < _ImpactEffectGroups.Num(); i++)
	{
		if (_ImpactEffectGroups[i].GetPhysicsMaterial()->SurfaceType == surfaceType)
		{
			// Play the effect on all clients
			Server_Unreliable_SpawnImpactEffectGroup(i, HitResult);
			break;
		}
	}
}

///////////////////////////////////////////////

/*
*
*/
bool AImpactEffectManager::Server_Unreliable_SpawnImpactEffectGroup_Validate(int ArrayPosition, FHitResult HitResult)
{ return true; }

void AImpactEffectManager::Server_Unreliable_SpawnImpactEffectGroup_Implementation(int ArrayPosition, FHitResult HitResult)
{
	// Get reference in effect groups
	UParticleSystem* particleSystemToPlay = _ImpactEffectGroups[ArrayPosition].GetImpactParticleSystem();
	UMaterialInstance* decalMaterialToSpawn = _ImpactEffectGroups[ArrayPosition].GetDecal();
	USoundBase* soundToPlay = _ImpactEffectGroups[ArrayPosition].GetSound();

	// Impact effect 
	FRotator particleRotation = FRotator::ZeroRotator;

	// Make random decal rotation
	FRotator decalRotation = UKismetMathLibrary::MakeRotFromX(HitResult.Normal);
	float ranFloat = FMath::RandRange(-180.0f, 180.0f);
	_rRandomDecalRotation = UKismetMathLibrary::MakeRotator(decalRotation.Roll, decalRotation.Pitch, ranFloat);

	// Spawn emitters
	if (particleSystemToPlay != NULL) { UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particleSystemToPlay, HitResult.ImpactPoint, particleRotation, FVector(1.0f, 1.0f, 1.0f)); }
	if (decalMaterialToSpawn != NULL) { UGameplayStatics::SpawnDecalAtLocation(GetWorld(), decalMaterialToSpawn, FVector(_fDecalSize, _fDecalSize, _fDecalSize), HitResult.ImpactPoint, decalRotation, _fDecalLifespan); }
	if (soundToPlay != NULL) { UGameplayStatics::PlaySoundAtLocation(GetWorld(), soundToPlay, HitResult.ImpactPoint); }
}
