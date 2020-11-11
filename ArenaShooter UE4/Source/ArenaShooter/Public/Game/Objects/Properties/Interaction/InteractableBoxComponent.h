// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "InteractableSphereComponent.h"

#include "InteractableBoxComponent.generated.h"

UCLASS(ClassGroup=(Collision), hidecategories = (Object, LOD, Lighting, TextureStreaming), editinlinenew, meta = (DisplayName = "Interactable Box Trigger", BlueprintSpawnableComponent))
class ARENASHOOTER_API UInteractableBoxComponent : public UBoxComponent
{
	GENERATED_BODY()

#pragma region Protected Variables

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		bool _bFocusOnly = false;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
		bool _bIsOverlapping = false;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
		FOverlappingInfo _OverlappingInfo;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
		UInteractionDataComponent* _InteractionData = NULL;

#pragma endregion Protected Variables

#pragma region Public Functions

public:	

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Startup

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	*	Sets default values for this component's properties
	*/
	UInteractableBoxComponent();

	/**
	*	Called when the game starts
	*/
	virtual void BeginPlay() override;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Current Frame

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	*	Called every frame.
	*/
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Interaction 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//
	UFUNCTION()
		void OnOverlapStay(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

#pragma endregion Public Functions

};
