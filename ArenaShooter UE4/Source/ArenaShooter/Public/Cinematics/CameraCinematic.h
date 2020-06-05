// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Actor.h"
#include "Structures.h"
#include "CameraCinematic.generated.h"

// *** EVENT DISPATCHERS / DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCinematicStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCinematicComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHeadZoom);

// *** CLASSES

class AIngameGameState;
class ACameraSpline;

UCLASS()
class ARENASHOOTER_API ACameraCinematic : public AActor
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
	ACameraCinematic();

protected:

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// References *****************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "References")
		AIngameGameState* _pIngameGameState = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "References")
		ACameraActor* _ViewCameraActor = NULL;
	
	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "References")
		TArray<ACameraSpline*> _tCameraSplines;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "References")
		ACameraSpline* _CameraSpline = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "References")
		int _iCurrentSpline = 0;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "References")
		int _iCurrentSplinePoint = 0;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "References")
		float _fLerpTime = 0.0f;

	// Cinematic Properties *******************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Cinematic Properties")
		E_CinematicType _eCinematicType = E_CinematicType::eGT_Opening;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Cinematic Properties")
		E_TeamNames _eTeam = E_TeamNames::eTN_Alpha;

	/*
	*
	*/
	UPROPERTY(BlueprintAssignable, Category = "Cinematic Properties")
		FOnCinematicStart _OnCinematicStart;

	/*
	*
	*/
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Cinematic Properties")
		FOnCinematicStart _OnCinematicComplete;

	// Head Zoom ******************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Head Zoom")
		FVector _HeadLocation = FVector::ZeroVector;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Head Zoom")
		FVector _ZoomStartLocation = FVector::ZeroVector;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Head Zoom")
		FRotator _HeadRotation = FRotator::ZeroRotator;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Head Zoom")
		FRotator _ZoomStartRotation = FRotator::ZeroRotator;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Head Zoom")
		bool _bZoomIntoHead = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Head Zoom")
		float _fZoomDuration = 0.25f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Head Zoom")
		float _fZoomFadeThresholdPercent = 0.5f;

	/*
	*
	*/
	UPROPERTY(BlueprintAssignable, Category = "Head Zoom")
		FHeadZoom _OnHeadZoomThreshold;

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

	///////////////////////////////////////////////

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

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


	/**
	* @summary:	Returns reference to the E_CinematicType on this instance (Opening/Closing cinematic)
	*
	* @return:	E_CinematicType
	*/
	UFUNCTION(BlueprintGetter)
		E_CinematicType GetCinematicType() { return _eCinematicType; }

	///////////////////////////////////////////////


	/*
	* @summary:	Starts the cinematic on this class instance. (Is being called from the GameState).
	*
	* @return:	void
	*/
	UFUNCTION()
		void StartCinematic();

};
