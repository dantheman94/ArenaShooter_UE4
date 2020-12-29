// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ButtonMainMenu.h"
#include "Structures.h"

#include "ButtonPlaylist.generated.h"

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API UButtonPlaylist : public UButtonMainMenu
{
	GENERATED_BODY()

protected:
	
	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FPlaylistInfo _ButtonInfo;

public:

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void Setup(FPlaylistInfo ButtonInfo, TArray<UButtonPlaylist*> FriendlyButtons);

	/*
	*
	*/
	virtual void Hover() override;

};
