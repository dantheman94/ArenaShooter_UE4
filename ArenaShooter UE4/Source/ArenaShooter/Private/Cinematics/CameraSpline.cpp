// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraSpline.h"

#include "Kismet/KismetMathLibrary.h"

// Startup ********************************************************************************************************************************

/**
* @summary:	Sets default values for this component's properties.
*
* @return:	Constructor
*/
ACameraSpline::ACameraSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create components
	USceneComponent* r = this->GetRootComponent();
	_Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	_Root->SetupAttachment(r);
	_Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	_Spline->SetupAttachment(_Root);
}

///////////////////////////////////////////////

/**
* @summary:	Called when the game starts or when spawned.
*
* @return:	virtual void
*/
void ACameraSpline::BeginPlay()
{
	Super::BeginPlay();
}

// Current Frame **************************************************************************************************************************

/**
* @summary:	Called every frame.
*
* @param:	float DeltaTime
*
* @return:	virtual void
*/
void ACameraSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

