// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionDataComponent.h"

#include "BaseCharacter.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Startup 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
*	Sets default values for this component's properties
*/
UInteractionDataComponent::UInteractionDataComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

/*

*/
void UInteractionDataComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractionDataComponent, _bLocked);
	DOREPLIFETIME(UInteractionDataComponent, _bInteractable);
}

/**
*	Called when the game starts
*/
void UInteractionDataComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Interaction 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool UInteractionDataComponent::Server_Reliable_SetIsLocked_Validate(bool IsLocked) { return true; }
void UInteractionDataComponent::Server_Reliable_SetIsLocked_Implementation(bool IsLocked)
{
	_bLocked = IsLocked;
}
void UInteractionDataComponent::SetIsLocked(bool Locked)
{
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
		_bLocked = Locked;
	else
		Server_Reliable_SetIsLocked(Locked);
}

bool UInteractionDataComponent::Server_Reliable_SetIsInteractable_Validate(bool IsInteractable) { return true; }
void UInteractionDataComponent::Server_Reliable_SetIsInteractable_Implementation(bool IsInteractable)
{
	_bInteractable = IsInteractable;
}
void UInteractionDataComponent::SetIsInteractable(bool Interactable)
{
	// Overwrite the object to not be interactable if it's locked
	bool interactable = Interactable;
	if (_bLocked)
		interactable = false;

	// Update _bInteractable value accordingly
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
		_bInteractable = interactable;
	else
		Server_Reliable_SetIsInteractable(interactable);
}

bool UInteractionDataComponent::Server_Reliable_OnInteract_Validate(ABaseCharacter* BaseCharacter, bool Destroy) { return true; }
void UInteractionDataComponent::Server_Reliable_OnInteract_Implementation(ABaseCharacter* BaseCharacter, bool Destroy)
{
	// Remove from character's interactable array
	if (BaseCharacter->GetInteractablesArray().Contains(this))
		BaseCharacter->GetInteractablesArray().Remove(this);
	
	// Call blueprint events
	OnInteract.Broadcast();
	///OnInteract();

	// Destroy/re-pool
	///if (Destroy)
	///	this->Destroy();
}