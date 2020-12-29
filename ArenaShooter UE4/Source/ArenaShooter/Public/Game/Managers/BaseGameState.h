// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Structures.h"

#include "BaseGameState.generated.h"

// *** EVENT DISPATCHERS / DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerInfoRefresh);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchCountdown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchCountdownCancel);

// *** CLASSES

UCLASS()
class ARENASHOOTER_API ABaseGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

protected:

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		TArray<class UTeamComponent*> _Teams;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		TArray<FPlayerInfo> _PlayerInfos;

	UPROPERTY(BlueprintAssignable)
		FOnPlayerInfoRefresh _OnPlayerInfoRefresh;

	UPROPERTY(BlueprintAssignable)
		FOnMatchCountdown _OnMatchCountdown;

	UPROPERTY(BlueprintAssignable)
		FOnMatchCountdownCancel _OnMatchCountdownCancel;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
		int _iMaxLobbySize = 0;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Lobby Properties")
		int _iLobbyTeamCap = 2;

public:

	UFUNCTION(BlueprintCallable)
		void UpdatePlayerList();

	UFUNCTION(BlueprintCallable)
		void UpdateTeamList();

	UFUNCTION()
		void SetMaxLobbySize(int Size);

	UFUNCTION(BlueprintPure)
		TArray<FPlayerInfo> GetPlayerInfos() { return _PlayerInfos; }

	UFUNCTION(BlueprintPure)
		int GetMaxLobbySize() { return _iMaxLobbySize; }

	UFUNCTION(BlueprintCallable)
		void DisconnectClientFromLobby();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reliable_CheckForGameOver();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_Reliable_SetGameMode(FGameTypeInfo GameTypeInfo);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_Reliable_SetMap(FMapInfo MapInfo);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_HostHasStartedMatchCountdown();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_Reliable_HostHasStartedMatchCountdown();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_HostHasCancelledMatchCountdown();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multicast_Reliable_HostHasCancelledMatchCountdown();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_PromoteToLeader(FPlayerInfo PlayerData);

	// Prematch Setup *************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_GenerateRandomGamemodeToPlay();

	UFUNCTION(BlueprintCallable)
		void GenerateRandomGamemodeToPlay();

	/*
	*
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_Reliable_GenerateRandomMapToPlay(FPlaylistInfo Playlist);

	UFUNCTION(Client, Unreliable)
		void sendmessage();
	
};
