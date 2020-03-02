// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Containers/UnrealString.h"
#include "UnrealNetwork.h"

#include "BaseCharacter.h"
#include "BaseHUD.h"

// Startup ********************************************************************************************************************************

/**
* @summary:	Sets default values for this actor's properties.
*
* @return:	Constructor
*/
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Actor replicates
	bReplicates = true;
	bReplicateMovement = true;
	SetRemoteRoleForBackwardsCompat(ROLE_AutonomousProxy);

	// Create components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	_InteractionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionTrigger"));
	_InteractionTrigger->SetupAttachment(RootComponent);
	_FocusMesh_Static = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FocusMesh_Static"));
	_FocusMesh_Static->SetupAttachment(_InteractionTrigger);
	_FocusMesh_Skeletal = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FocusMesh_Skeletal"));
	_FocusMesh_Skeletal->SetupAttachment(_InteractionTrigger);
}

///////////////////////////////////////////////

void AInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractable, _bInteractable);
}

///////////////////////////////////////////////

/**
* @summary:	Called when the game starts or when spawned.
*
* @return:	virtual void
*/
void AInteractable::BeginPlay()
{
	Super::BeginPlay();

	// Add overlap events
	_InteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &AInteractable::OnOverlapBegin);
	_InteractionTrigger->OnComponentEndOverlap.AddDynamic(this, &AInteractable::OnOverlapEnd);
}

// Current Frame **************************************************************************************************************************

/**
* @summary:	Called every frame.
*
* @param:	float DeltaTime
*
* @return:	virtual void
*/
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Interaction ****************************************************************************************************************************

/*
*
*/
void AInteractable::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Don't proceed unless we are allowed to be interacted with
	if (_bInteractable == false) { return; }

	///GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("overlap!") + OtherComp->GetName());

	// Matching collision type?
	ECollisionChannel characterMovementChannel = ECollisionChannel::ECC_GameTraceChannel12;
	if (OtherComp->GetCollisionObjectType() == ECC_Pawn || OtherComp->GetCollisionObjectType() == characterMovementChannel)
	{
		// Cast to correct character type
		ABaseCharacter* baseCharacter = Cast<ABaseCharacter>(OtherActor);
		if (baseCharacter == NULL) { return; }
	
		// Add to interactables array
		baseCharacter->AddToInteractablesArray(this);
		AInteractable* interactable = baseCharacter->CalculateFocusInteractable();
		
		// Display interactable info to player's screen/HUD
		APlayerController* playerController = Cast<APlayerController>(baseCharacter->GetController());
		if (playerController == NULL) { return; }
		ABaseHUD* hud = Cast<ABaseHUD>(playerController->GetHUD());
		if (hud == NULL) { return; }
		hud->SetNewFocusInteractable(interactable);
		hud->SetWidgetInteractable(interactable);
	}
}

///////////////////////////////////////////////

/*
*
*/
void AInteractable::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Cast to correct character type
	ABaseCharacter* baseCharacter = Cast<ABaseCharacter>(OtherActor);
	if (baseCharacter == NULL) { return; }

	// Remove from interactables array
	baseCharacter->RemoveFromInteractablesArray(this);
	AInteractable* interactable = baseCharacter->CalculateFocusInteractable();

	// Display interactable info to player's screen/HUD
	APlayerController* playerController = Cast<APlayerController>(baseCharacter->GetController());
	if (playerController == NULL) { return; }
	ABaseHUD* hud = Cast<ABaseHUD>(playerController->GetHUD());
	if (hud == NULL) { return; }
	hud->SetNewFocusInteractable(interactable);
	hud->SetWidgetInteractable(interactable);
}

///////////////////////////////////////////////

/*
*
*/
bool AInteractable::Server_Reliable_OnInteract_Validate(ABaseCharacter* BaseCharacter, bool Destroy = true)
{ return true; }

void AInteractable::Server_Reliable_OnInteract_Implementation(ABaseCharacter* BaseCharacter, bool Destroy = true)
{
	// Remove from character's interactable array
	if (BaseCharacter->GetInteractablesArray().Contains(this))
	{ BaseCharacter->GetInteractablesArray().Remove(this); }

	// Destroy/re-pool
	if (Destroy) { this->Destroy(); }
}

///////////////////////////////////////////////

/*
*
*/
bool AInteractable::SetIsInteractable_Validate(bool IsInteractable)
{ return true; }

void AInteractable::SetIsInteractable_Implementation(bool IsInteractable)
{
	_bInteractable = IsInteractable;
}

