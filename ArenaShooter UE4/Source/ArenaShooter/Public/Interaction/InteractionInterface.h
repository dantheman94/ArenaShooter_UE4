// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "InteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARENASHOOTER_API IInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) void OnBeginPlay();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) void OnBeginFocus();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) void OnEndFocus();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) void OnInteractionStart();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) void OnInteractionComplete();

};
