// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Structures.h"

#include "BaseGameState.generated.h"

// *** EVENT DISPATCHERS / DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerInfoRefresh);

// *** CLASSES

UCLASS()
class ARENASHOOTER_API ABaseGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		TArray<FPlayerInfo> _PlayerInfos;

	UPROPERTY(BlueprintAssignable)
		FOnPlayerInfoRefresh _OnPlayerInfoRefresh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		int _iMaxLobbySize = 0;

public:

	UFUNCTION(BlueprintCallable)
		void UpdatePlayerList();

	///////////////////////////////////////////////

	UFUNCTION()
		void SetMaxLobbySize(int Size);

};
