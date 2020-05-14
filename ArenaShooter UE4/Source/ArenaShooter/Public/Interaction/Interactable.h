// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;

UCLASS()
class ARENASHOOTER_API AInteractable : public AActor
{
	GENERATED_BODY()
	
public:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Startup ********************************************************************************************************************************

	/**
	* @summary:	Sets default values for this actor's properties.
	*
	* @return:	Constructor
	*/
	AInteractable();

	///////////////////////////////////////////////

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USphereComponent* _InteractionTrigger;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UStaticMeshComponent* _FocusMesh_Static;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USkeletalMeshComponent* _FocusMesh_Skeletal;

protected:

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

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FName _PickupName = TEXT("< MISSING NAME >");

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
		bool _bInteractable = true;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<AActor> _OnUsedActorClass;

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

	// Interaction ****************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void SetIsInteractable(bool IsInteractable);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_OnInteract(ABaseCharacter* BaseCharacter, bool Destroy = true);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		FName GetInteractablePickupName() { return _PickupName; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		TSubclassOf<AActor> GetOnUsedActor() { return _OnUsedActorClass; }

};
