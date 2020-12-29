// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structures.h"

#include "PlaylistPreview.generated.h"

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API UPlaylistPreview : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FPlaylistInfo _PlaylistInfo;

public:

	UFUNCTION(BlueprintCallable)
		void SetPlaylistInfo(FPlaylistInfo info) { _PlaylistInfo = info; }

};
