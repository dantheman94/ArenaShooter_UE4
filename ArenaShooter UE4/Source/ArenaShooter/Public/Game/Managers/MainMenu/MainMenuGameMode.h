// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/VerticalBox.h"
#include "Game/Managers/BaseGameMode.h"
#include "Engine/DataTable.h"

#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API AMainMenuGameMode : public ABaseGameMode
{
	GENERATED_BODY()
	
public:

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void StartMatchmaking();
};
