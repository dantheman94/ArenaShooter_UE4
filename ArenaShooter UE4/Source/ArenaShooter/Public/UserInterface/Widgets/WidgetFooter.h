// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "WidgetFooter.generated.h"

// *** ENUMS

UENUM(BlueprintType)
enum class E_FooterOptions : uint8
{
	eFO_Select UMETA(DisplayName = "Select"),
	eFO_Back UMETA(DisplayName = "Back"),
	eFO_Roster UMETA(DisplayName = "Toggle Roster"),
	eFO_Profile UMETA(DisplayName = "Profile"),
	eFO_LeaveLobby UMETA(DisplayName = "Leave Lobby"),
	eFO_CancelMatchmaking UMETA(DisplayName = "Cancel Matchmaking"),
	eFO_ViewScoreboard UMETA(DisplayName = "View Scoreboard"),
};

// *** CLASSES

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API UWidgetFooter : public UUserWidget
{
	GENERATED_BODY()

protected:

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Properties")
		FText _DescriptorText;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Properties")
		TArray<E_FooterOptions> _FooterOptions;

public:

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void SetDescriptorText(FText Message) { _DescriptorText = Message; }
	 
};
