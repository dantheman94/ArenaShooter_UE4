// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableSphereComponent.h"

#include "Components/SceneComponent.h"

/*
	Sets default values for this component's properties
*/
UInteractableSphereComponent::UInteractableSphereComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// Create overlap events
	this->SetGenerateOverlapEvents(true);
	this->OnComponentBeginOverlap.AddDynamic(this, &UInteractableSphereComponent::OnOverlapBegin);
	this->OnComponentEndOverlap.AddDynamic(this, &UInteractableSphereComponent::OnOverlapEnd);
}

/*
	Called when the game starts
*/
void UInteractableSphereComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get component references
	_InteractionData = GetOwner()->FindComponentByClass<UInteractionDataComponent>();
}

/*
	Called every frame
*/
void UInteractableSphereComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (_bIsOverlapping)
	{
		UPrimitiveComponent* overlappedComp = _OverlappingInfo.OverlappedComp;
		AActor* otherActor = _OverlappingInfo.OtherActor;
		UPrimitiveComponent* otherComp = _OverlappingInfo.OtherComp;
		int32 otherBodyIndex = _OverlappingInfo.OtherBodyIndex;
		OnOverlapStay(overlappedComp, otherActor, otherComp, otherBodyIndex);
	}
}

void UInteractableSphereComponent::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Don't proceed unless we are allowed to be interacted with
	if (_InteractionData == NULL)
		return;
	if (!_InteractionData->IsInteractable())
		return;
	///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("overlap! ") + OtherComp->GetName());

	// Matching collision type?
	ECollisionChannel characterMovementChannel = ECollisionChannel::ECC_GameTraceChannel12;
	if (OtherComp->GetCollisionObjectType() == ECC_Pawn || OtherComp->GetCollisionObjectType() == characterMovementChannel)
	{
		// Cast to correct character type
		ABaseCharacter* baseCharacter = Cast<ABaseCharacter>(OtherActor);
		if (baseCharacter == NULL)
			return;

		// Add to interactables array & refocus
		_InteractionData->SetIsInteractable(!_bFocusOnly);
		baseCharacter->AddToInteractablesArray(_InteractionData);
		baseCharacter->FocusInteractableToHUD();

		// Setup for OnOverlapStay() calls in Tick()
		_OverlappingInfo.OverlappedComp = OverlappedComp;
		_OverlappingInfo.OtherActor = OtherActor;
		_OverlappingInfo.OtherComp = OtherComp;
		_OverlappingInfo.OtherBodyIndex = OtherBodyIndex;
		_bIsOverlapping = true;
	}
}

void UInteractableSphereComponent::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Sanity checks
	if (_InteractionData == NULL)
		return;
	if (!_InteractionData->IsInteractable())
		return;
	ABaseCharacter* baseCharacter = Cast<ABaseCharacter>(OtherActor);
	if (baseCharacter == NULL)
		return;

	// Remove the reference of the interaction data component from the potential interactables array in the character
	bool b = baseCharacter->RemoveFromInteractablesArray(_InteractionData);
	baseCharacter->FocusInteractableToHUD();
	///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, b ? TEXT("Success!") : TEXT("Failure :("));

	// Setup for OnOverlapStay() calls in Tick()
	_bIsOverlapping = false;
	_OverlappingInfo.OverlappedComp = NULL;
	_OverlappingInfo.OtherActor = NULL;
	_OverlappingInfo.OtherComp = NULL;
	_OverlappingInfo.OtherBodyIndex = NULL;
}

void UInteractableSphereComponent::OnOverlapStay(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Don't proceed unless we are allowed to be interacted with
	if (_InteractionData == NULL)
		return;
	if (!_InteractionData->IsInteractable())
		return;
	if (OtherComp == NULL || OtherActor == NULL)
		return;

	// Matching collision type?
	ECollisionChannel characterMovementChannel = ECollisionChannel::ECC_GameTraceChannel12;
	if (OtherComp->GetCollisionObjectType() == ECC_Pawn || OtherComp->GetCollisionObjectType() == characterMovementChannel)
	{
		// Cast to correct character type
		ABaseCharacter* baseCharacter = Cast<ABaseCharacter>(OtherActor);
		if (baseCharacter == NULL)
			return;

		// Add to interactables array & refocus
		if (!baseCharacter->GetInteractablesArray().Contains(_InteractionData))
			baseCharacter->AddToInteractablesArray(_InteractionData);
		baseCharacter->FocusInteractableToHUD();
	}
}
