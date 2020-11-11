// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "Structures.h"

#include "SpawnVolume.generated.h"

// *** CLASSES

UCLASS()
class ARENASHOOTER_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Startup

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @summary:	Sets default values for this component's properties.
	*
	* @return:	Constructor
	*/
	ASpawnVolume();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UBoxComponent* _SpawnBounds;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UTextRenderComponent* _Text_Team;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UTextRenderComponent* _Text_Gamemode;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UTextRenderComponent* _Text_SpawnVolume;

protected:

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Properties

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Settings")
		TArray<E_GameTypes> _GameModes;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Settings")
		E_TeamNames _Team = E_TeamNames::eTN_Alpha;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Settings")
		bool _bStartingSpawnVolume = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Properties")
		FVector _BoundsHalfExtents = FVector(200.0f, 200.0f, 200.0f);

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Properties")
		float _fSpawnWeight = 1.0f;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Settings

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Settings")
		bool _bTraceOverrideZExtent = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Settings")
		float _fTraceZExtentOffset = 1000.0f;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Settings")
		bool _bUseMultitraceOnFail = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Settings")
		int _iMaxMultitraceIterations = 200;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Settings")
		bool _bUseComplexTrace = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Settings")
		bool _bUseHalfTraceOnFail = true;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Settings")
		bool _bUsedHalfHeight = false;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Startup

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @summary:	Called when the game starts or when spawned.
	*
	* @return:	virtual void
	*/
	virtual void BeginPlay() override;

public:

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Current Frame

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @summary:	Called every frame.
	*
	* @param:	float DeltaTime
	*
	* @return:	virtual void
	*/
	virtual void Tick(float DeltaTime) override;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Settings

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void SetupGraphics(UTextRenderComponent* GametypeName, UTextRenderComponent* TeamName, UTextRenderComponent* SpawnName);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Spawning

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		FVector CalculateSpawnLocation(FVector TraceStartLocation, bool TraceComplex);

	/*
	*
	*/
	UFUNCTION()
		FVector GetRandomLocationWithinVolumeBounds();

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		bool SpawnActorWithinVolume(TSubclassOf<AActor> ActorClass, float ActorVerticalOffset);

	// TEMPORARILY PLACED HERE FOR EASE OF USE -> TO BE MOVED INTO A SEPARATE CLASS LATER
	const FString EnumToString(const TCHAR* Enum, int32 EnumValue)
	{
		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, Enum, true);
		if (!EnumPtr)
			return NSLOCTEXT("Invalid", "Invalid", "Invalid").ToString();

#if WITH_EDITOR
		return EnumPtr->GetDisplayNameTextByIndex(EnumValue).ToString();
#else
		return EnumPtr->GetEnumName(EnumValue);
#endif
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Properties

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		TArray<E_GameTypes> GetGameModes() { return _GameModes; }

	/*
	*
	*/
	UFUNCTION()
		bool IsStartingspawnVolume() { return _bStartingSpawnVolume; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure) E_TeamNames GetTeam() const { return _Team; }

};
