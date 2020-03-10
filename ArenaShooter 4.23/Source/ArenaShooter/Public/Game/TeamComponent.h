// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/Texture2D.h"

#include "TeamComponent.generated.h"

// *** ENUMS

UENUM(BlueprintType)
enum class E_TeamNames : uint8
{
	eTN_Alpha UMETA(DisplayName = "Alpha"),
	eTN_Bravo UMETA(DisplayName = "Bravo"),
	eTN_Charlie UMETA(DisplayName = "Charlie"),
	eTN_Delta UMETA(DisplayName = "Delta")
};

class ABasePlayerController;

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ARENASHOOTER_API UTeamComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	/**
	* @summary:	Sets default values for this component's properties.
	*
	* @return:	Constructor
	*/
	UTeamComponent();

protected:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Startup ********************************************************************************************************************************

	/**
	* @summary:	Called when the game starts or when spawned.
	*
	* @return:	virtual void
	*/
	virtual void BeginPlay() override;

	///////////////////////////////////////////////

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// Team Properties ************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		E_TeamNames _eTeam = E_TeamNames::eTN_Alpha;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FLinearColor _cTeamColour = FLinearColor::White;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UTexture2D* _TeamLogo = NULL;

	// Team Players ***************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		TArray<ABasePlayerController*> _TeamPlayers;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		ABasePlayerController* _TeamMVP;

	// Score **********************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		int _iTeamScore = 0;

public:	

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Current Frame **************************************************************************************************************************

	/**
	* @summary:	Called every frame.
	*
	* @param:	float DeltaTime
	*
	* @return:	virtual void
	*/
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Team Properties ************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		E_TeamNames GetName() { return _eTeam; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		FLinearColor GetTeamColour() { return _cTeamColour; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		UTexture2D* GetTeamLogo() { return _TeamLogo; }

	// Team Players ***************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		ABasePlayerController* GetPlayerMVP() { return _TeamMVP; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		TArray<ABasePlayerController*> GetPlayers() { return _TeamPlayers; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetMaxTeamSize();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void AddPlayerToTeam(ABasePlayerController* PlayerController);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_AddPlayerToTeam(ABasePlayerController* PlayerController);

	// Score **********************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		int GetTeamScore() { return _iTeamScore; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void AddToTeamScore(int Additive);

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_AddToTeamScore(int Additive);

	///////////////////////////////////////////////

};
