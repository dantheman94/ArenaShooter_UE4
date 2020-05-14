// Fill out your copyright notice in the Description page of Project Settings.

#include "TeamComponent.h"

#include "BaseGameMode.h"
#include "BaseGameState.h"
#include "BasePlayerController.h"
#include "UnrealNetwork.h"

// Startup ********************************************************************************************************************************

/**
* @summary:	Sets default values for this component's properties.
*
* @return:	Constructor
*/
UTeamComponent::UTeamComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Component replicates
	SetIsReplicated(true);
}

///////////////////////////////////////////////

/**
* @summary:	Called when the game starts or when spawned.
*
* @return:	virtual void
*/
void UTeamComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

///////////////////////////////////////////////

void UTeamComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTeamComponent, _TeamPlayers);
	DOREPLIFETIME(UTeamComponent, _TeamMVP);
	DOREPLIFETIME(UTeamComponent, _iTeamScore);
}

// Current Frame **************************************************************************************************************************

/**
* @summary:	Called every frame.
*
* @param:	float DeltaTime
*
* @return:	virtual void
*/
void UTeamComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// Team Players ***************************************************************************************************************************

/*
*
*/
int UTeamComponent::GetMaxTeamSize()
{
	int ret = 0;

	// Gamemode only exists on server, so this will only return a valid value if server authority called this
	ABaseGameMode* gameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode());
	if (gameMode != NULL)
	{
		ret = gameMode->GetMaxLobbySize();
	}

	return ret;
}

///////////////////////////////////////////////

/*
*
*/
void UTeamComponent::AddPlayerToTeam(ABasePlayerController* PlayerController)
{
	// Get attach parent
	AActor* owner = GetOwner();
	if (owner->GetLocalRole() == ROLE_Authority)
	{
		_TeamPlayers.Add(PlayerController);
	}

	// Run this again on the server
	else { Server_Reliable_AddPlayerToTeam(PlayerController); }
}

/*
*
*/
bool UTeamComponent::Server_Reliable_AddPlayerToTeam_Validate(ABasePlayerController* PlayerController)
{ return true; }

void UTeamComponent::Server_Reliable_AddPlayerToTeam_Implementation(ABasePlayerController* PlayerController)
{
	AddPlayerToTeam(PlayerController);
}

// Score **********************************************************************************************************************************

/*
*
*/
void UTeamComponent::AddToTeamScore(int Additive)
{
	// Get attach parent
	AActor* owner = GetOwner();
	if (owner->GetLocalRole() == ROLE_Authority)
	{ 
		_iTeamScore += Additive; 

		// Get GameState and cast it to the correct type
		AGameStateBase* gs = GetWorld()->GetGameState();
		ABaseGameState* gameState = Cast<ABaseGameState>(gs);
		if (gameState != NULL)
		{
			// Check for game over
			gameState->Server_Reliable_CheckForGameOver();
 		}
	}

	// Run this again on the server
	else { Server_Reliable_AddToTeamScore(Additive); }
}

/*
*
*/
bool UTeamComponent::Server_Reliable_AddToTeamScore_Validate(int Additive)
{ return true; }

void UTeamComponent::Server_Reliable_AddToTeamScore_Implementation(int Additive)
{
	AddToTeamScore(Additive);
}

