// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InteractionDataComponent.generated.h"

// *** EVENT DISPATCHERS / DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteract);

UCLASS( ClassGroup=(Custom), BlueprintType, Blueprintable, hidecategories = (Object, LOD, Lighting, TextureStreaming), editinlinenew, meta=(DisplayName = "Interaction Properties", BlueprintSpawnableComponent) )
class ARENASHOOTER_API UInteractionDataComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	/*
		The name of the object that appears in the HUD widgets when this object is being "focused"
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FName _PickupName = TEXT("< MISSING NAME >");

	/*
		Are the interaction triggers associated with the actor that this component is attached to enabled for interaction?
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
		bool _bLocked = false;

	/*
		Are the interaction triggers associated with the actor that this component is attached to enabled for interaction?
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
		bool _bInteractable = true;

	/*
		Spawns this actor when an interaction is fully complete.
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<AActor> _OnUsedActorClass;

	/*
		Represents whether any of the static/skeletal root meshes should play physics simulations at startup
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		bool _bSimulatePhysicsOnStart = true;

public:

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Startup 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
		Sets default values for this component's properties
	*/
	UInteractionDataComponent();

	/*
		
	*/
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

	/*
		Called when the game starts
	*/
	virtual void BeginPlay() override;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Interaction 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*

	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetIsLocked(bool IsLocked);
	UFUNCTION(BlueprintCallable) void SetIsLocked(bool Locked);

	/*

	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_SetIsInteractable(bool IsInteractable);
	UFUNCTION(BlueprintCallable) void SetIsInteractable(bool Interactable);

	/*

	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_OnInteract(ABaseCharacter* BaseCharacter, bool Destroy = true);

	/*

	*/
	UFUNCTION(BlueprintCallable) void SetSimulatePhysicsOnStart(bool Simulate) { _bSimulatePhysicsOnStart = Simulate; }

	// Getters
	UFUNCTION(BlueprintPure) FName GetInteractablePickupName() const { return _PickupName; }
	UFUNCTION(BlueprintPure) bool IsInteractable() const { return _bInteractable; }
	UFUNCTION(BlueprintPure) TSubclassOf<AActor> GetOnUsedActor() const { return _OnUsedActorClass; }
	UFUNCTION(BlueprintPure) bool IsSimulatingPhysicsOnStart() const { return _bSimulatePhysicsOnStart; }

	// Blueprint override events
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnInteract OnInteract;

	///UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) void OnBeginFocus();
	///UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) void OnEndFocus();
	///UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) void OnInteract();
};