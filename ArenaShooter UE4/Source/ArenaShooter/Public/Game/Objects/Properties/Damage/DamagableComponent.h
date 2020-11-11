// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "DamagableInterface.h"

#include "DamagableComponent.generated.h"

USTRUCT(BlueprintType)
struct FDamagableParticleSystem
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		UParticleSystem* _pOnTransitionParticleSystem;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FTransform _fParticleSystemTransform;
};

USTRUCT(BlueprintType)
struct FDamagableInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float StateEnterHealthThreshold = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		UStaticMesh* _pStaticMesh;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<FDamagableParticleSystem> _ParticleSystems;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent, DisplayName = "Damagable Static Mesh"))
class ARENASHOOTER_API UDamagableComponent : public UStaticMeshComponent, public IDamagableInterface
{
	GENERATED_BODY()

public:

	// Sets default values for this component's properties
	UDamagableComponent();

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Health, meta = (DisplayName = "Health"))
		float _DamageHitPoints = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int _iCurrentDamageState = 0;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		bool _bCanReceiveDamage = true;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<FDamagableInfo> _DestructibleData;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};