// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structures.generated.h"

USTRUCT(BlueprintType)
struct FAnimationListArms
{
	GENERATED_BODY()

		FAnimationListArms() {}

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_FirstPickup = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_Equip = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_Unequip = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_Inspect = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadEjectMag = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadNewMag = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadChamberRound = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadFullNotEmpty = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadFullEmpty = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadDuelRight = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadDuelLeft = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_FireProjectileHipfire = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_FireProjectileADS = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ProjectileMisfire = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_PumpProjectile = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_LoopingReloadEnter = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_LoopingReload = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_LoopingReloadExit = NULL;

};

USTRUCT(BlueprintType)
struct FAnimationListWeapon
{
	GENERATED_BODY()

		FAnimationListWeapon() {}

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_FirstPickup = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_Inspect = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadEjectMag = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadNewMag = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadChamberRound = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadFullNotEmpty = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadFullEmpty = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadDuelRight = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadDuelLeft = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_FireProjectileHipfire = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_FireProjectileADS = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ProjectileMisfire = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_PumpProjectile = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_LoopingReloadEnter = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_LoopingReload = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_LoopingReloadExit = NULL;

};