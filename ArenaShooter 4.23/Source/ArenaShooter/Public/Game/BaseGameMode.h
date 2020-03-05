// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Structures.h"

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

// *** CLASSES

class ABasePlayerController;
class ABaseGameState;

UCLASS()
class ARENASHOOTER_API ABaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ABaseGameMode();

protected:

	// Startup ********************************************************************************************************************************

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	/**
	* @summary:	Called when the game starts or when spawned.
	*
	* @return:	virtual void
	*/
	virtual void BeginPlay() override;

	TArray<ABasePlayerController*> _ConnectedBasePlayerControllers;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Lobby Properties")
		int _iMaxLobbySize = 10;

public:

	virtual void PostLogin(APlayerController* NewPlayer) override;

	///////////////////////////////////////////////

	virtual void Logout(AController* ExitingController) override;

	///////////////////////////////////////////////

	TArray<ABasePlayerController*>GetConnectedPlayerControllers() { return _ConnectedBasePlayerControllers; }

	///////////////////////////////////////////////

	int GetMaxLobbySize() { return _iMaxLobbySize; }

};
