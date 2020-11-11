// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "InteractionDataComponent.h"

#include "InteractableSphereComponent.generated.h"

USTRUCT(BlueprintType)
struct FOverlappingInfo
{
	GENERATED_BODY()

		FOverlappingInfo() {}

public:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UPrimitiveComponent* OverlappedComp;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		AActor* OtherActor;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UPrimitiveComponent* OtherComp;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		int32 OtherBodyIndex;
};

UCLASS(ClassGroup=(Collision), BlueprintType, Blueprintable, hidecategories = (Object, LOD, Lighting, TextureStreaming), editinlinenew, meta = (DisplayName = "Interactable Spherical Trigger", BlueprintSpawnableComponent))
class ARENASHOOTER_API UInteractableSphereComponent : public USphereComponent
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
	UInteractableSphereComponent();

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
