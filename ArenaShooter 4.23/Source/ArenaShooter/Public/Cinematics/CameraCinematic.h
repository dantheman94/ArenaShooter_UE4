// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structures.h"
#include "CameraCinematic.generated.h"

// *** EVENT DISPATCHERS / DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCinematicStart);
// *** CLASSES

class AIngameGameState;

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
	UPROPERTY(EditAnywhere, Category = "References")
		AActor* _ViewCameraActor = NULL;

	// Cinematic Properties *******************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Cinematic Properties")
		E_CinematicType _eCinematicType = E_CinematicType::eGT_Opening;
	
	/*
	*
	*/
	UPROPERTY(BlueprintAssignable, Category = "Cinematic Properties")
		FOnCinematicStart _OnCinematicStart;

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

	/*
	*
	*/
	UFUNCTION(BlueprintGetter)
		E_CinematicType GetCinematicType() { return _eCinematicType; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void StartCinematic();

};
