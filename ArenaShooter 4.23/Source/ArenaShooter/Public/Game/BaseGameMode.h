// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseGameMode.generated.h"

// *** ENUMS

UENUM(BlueprintType)
enum class E_TeamNames : uint8
{
	eTN_Alpha UMETA(DisplayName = "Alpha"),
	eTN_Bravo UMETA(DisplayName = "Bravo"),
	eTN_Charlie UMETA(DisplayName = "Charlie"),
	eTN_Delta UMETA(DisplayName = "Delta")
};

// *** STRUCTS

USTRUCT(BlueprintType)
struct FTeamProperties
{
	GENERATED_BODY()

		FTeamProperties() {}

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		E_TeamNames _eTeam = E_TeamNames::eTN_Alpha;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FLinearColor _cTeamColour = FLinearColor::White;

	E_TeamNames GetTeam()
	{ return _eTeam; }

	FLinearColor GetTeamColour()
	{ return _cTeamColour; }

};

// *** CLASSES

UCLASS()
class ARENASHOOTER_API ABaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

};
