// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"

#include "Interactable.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;

UCLASS()
class ARENASHOOTER_API AInteractable : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
#pragma region Public Variables

public:

	//
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USphereComponent* _InteractionTrigger;

	//
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UStaticMeshComponent* _FocusMesh_Static;

	//
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		USkeletalMeshComponent* _FocusMesh_Skeletal;

	//
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere,
		meta = (EditCondition = "_bOverwriteInteractionTrigger"))
		UShapeComponent* _OverwriteInteractionTrigger;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TScriptInterface<IInteractionInterface> _InteractionInterface;

#pragma endregion Public Variables

#pragma region Protected Variables

protected:

	//
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FName _PickupName = TEXT("< MISSING NAME >");

	//
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
		bool _bInteractable = true;

	//
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<AActor> _OnUsedActorClass;

	//
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool _bOverwriteInteractionTrigger = false;

#pragma endregion Protected Variables

#pragma region Public Functions

public:

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Startup 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Sets default values for this actor's properties.
	AInteractable();

	//
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

	// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Current Frame 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Called every frame
	virtual void Tick(float DeltaTime) override;

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
	UFUNCTION(Server, Reliable, WithValidation)
		void SetIsInteractable(bool IsInteractable);

	//
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_OnInteract(ABaseCharacter* BaseCharacter, bool Destroy = true);

	//
	UFUNCTION(BlueprintPure) FName GetInteractablePickupName() const { return _PickupName; }

	//
	UFUNCTION(BlueprintPure) TSubclassOf<AActor> GetOnUsedActor() const { return _OnUsedActorClass; }

	//
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void OnBeginPlay();  // prototype declaration
	virtual void OnBeginPlay_Implementation();  // actual implementation of our interact method

#pragma endregion Public Functions

};
