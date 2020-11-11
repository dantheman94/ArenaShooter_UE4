// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponAnimInstance.generated.h"

class AWeapon;

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API UWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Current")
		AWeapon* _Weapon = NULL;

public:
	
	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void SetWeapon(AWeapon* Weapon) { _Weapon = Weapon; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		AWeapon* GetWeapon() const { return _Weapon; }
};
