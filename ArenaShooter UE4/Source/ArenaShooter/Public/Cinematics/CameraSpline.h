// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "CameraSpline.generated.h"

// *** CLASSES

UCLASS()
class ARENASHOOTER_API ACameraSpline : public AActor
{
	GENERATED_BODY()
	
public:	

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Startup ********************************************************************************************************************************

	/**
	* @summary:	Sets default values for this component's properties.
	*
	* @return:	Constructor
	*/
	ACameraSpline();

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USceneComponent* _Root = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USplineComponent* _Spline = NULL;

protected:

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// Cinematic Properties *******************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Cinematic Properties")
		float _fDelayBeforeMoving = 0.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Cinematic Properties")
		float _fLerpDuration = 1.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Cinematic Properties")
		float _fTransitionDelay = 1.0f;

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

public:	

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Current Frame **************************************************************************************************************************

	/**
	* @summary:	Called every frame.
	*
	* @param:	float DeltaTime
	*
	* @return:	virtual void
	*/
	virtual void Tick(float DeltaTime) override;

	// Cinematic Properties *******************************************************************************************************************

	UFUNCTION(BlueprintGetter)
		USplineComponent* GetSpline() { return _Spline; }

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		float GetLerpDuration() { return _fLerpDuration; }

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		float GetTransitionDelay() { return _fTransitionDelay; }

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		float GetPreDelay() { return _fDelayBeforeMoving; }

	/*
	*
	*/
	UFUNCTION(BlueprintSetter)
		void SetPreDelay(float NewValue) { _fDelayBeforeMoving = NewValue; }

};
