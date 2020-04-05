// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraCinematic.h"

#include "BasePlayerController.h"
#include "IngameGameState.h"
#include "UnrealNetwork.h"

// Startup ********************************************************************************************************************************

/**
* @summary:	Sets default values for this component's properties.
*
* @return:	Constructor
*/
ACameraCinematic::ACameraCinematic()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

/**
* @summary:	Called when the game starts or when spawned.
*
* @return:	virtual void
*/
void ACameraCinematic::BeginPlay()
{
	Super::BeginPlay();

	// Get GameState reference
	if (_pIngameGameState == NULL)
	{
		AGameStateBase* gameState = GetWorld()->GetGameState();
		_pIngameGameState = Cast<AIngameGameState>(gameState);
	}
	
	// Add cinematic to array
	if (_pIngameGameState != NULL) { _pIngameGameState->Server_Reliable_AddCinematicToArray(this); }
}

///////////////////////////////////////////////

void ACameraCinematic::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	///DOREPLIFETIME(ACameraCinematic, _bCanTryToVentCooldown);
}

// Current Frame **************************************************************************************************************************

/**
* @summary:	Called every frame.
*
* @param:	float DeltaTime
*
* @return:	virtual void
*/
void ACameraCinematic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

///////////////////////////////////////////////

/*
*
*/
void ACameraCinematic::StartCinematic()
{
	if (_ViewCameraActor == NULL) { return; }

	// This is so in blueprint, we can use custom UMG for fade ins or black bars without explicitly calling them in code
	_OnCinematicStart.Broadcast();

	// Initialize player controller view to starting actor
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (playerController != NULL)
	{
		playerController->SetViewTarget(_ViewCameraActor);
	}
}

