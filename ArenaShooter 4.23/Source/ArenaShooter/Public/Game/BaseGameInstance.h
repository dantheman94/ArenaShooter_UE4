// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Online.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Structures.h"
#include "UnrealNetwork.h"
#include "UObject/CoreOnline.h"
#include "UObject/NoExportTypes.h"

#include "BaseGameInstance.generated.h"

#define SETTING_SERVER_NAME FName(TEXT("SERVERNAMEKEY"))
#define SETTING_SERVER_IS_PROTECTED FName(TEXT("SERVERSERVERISPASSWORDPROTECTEDKEY"))
#define SETTING_SERVER_PROTECT_PASSWORD FName(TEXT("SERVERPROTECTPASSWORDKEY"))

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIPDelegate, FString, IP);

// A custom struct to be able to access the Session results in blueprint
USTRUCT(BlueprintType)
struct FCustomBlueprintSessionResult
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Blueprint Session Result")
		FString _SessionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Blueprint Session Result")
		bool _bIsLan;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Blueprint Session Result")
		int32 _CurrentNumberOfPlayers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Blueprint Session Result")
		int32 _MaxNumberOfPlayers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Blueprint Session Result")
		bool _bIsPasswordProtected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Blueprint Session Result")
		FString _SessionPassword;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Blueprint Session Result")
		int32 _Ping;
};

USTRUCT(BlueprintType)
struct FBPUniqueNetId
{
	GENERATED_USTRUCT_BODY()

private:

	bool _bUseDirectPointer;
	
public:

	TSharedPtr<const FUniqueNetId> UniqueNetId;
	const FUniqueNetId * UniqueNetIdPtr;

	void SetUniqueNetId(const TSharedPtr<const FUniqueNetId> &ID)
	{
		_bUseDirectPointer = false;
		UniqueNetIdPtr = nullptr;
		UniqueNetId = ID;
	}

	void SetUniqueNetId(const FUniqueNetId *ID)
	{
		_bUseDirectPointer = true;
		UniqueNetIdPtr = ID;
	}

	bool IsValid() const
	{
		if (_bUseDirectPointer && UniqueNetIdPtr != nullptr) { return true; }
		else if (UniqueNetId.IsValid()) { return true; }
		else { return false; }
	}

	const FUniqueNetId* GetUniqueNetId() const
	{
		if (_bUseDirectPointer && UniqueNetIdPtr != nullptr) { return (UniqueNetIdPtr); }
		else if (UniqueNetId.IsValid()) { return UniqueNetId.Get(); }
		else { return nullptr; }
	}

	FBPUniqueNetId()
	{
		_bUseDirectPointer = false;
		UniqueNetIdPtr = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FSteamFriendInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steam Friend Info")
		UTexture2D* PlayerAvatar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steam Friend Info")
		FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steam Friend Info")
		FBPUniqueNetId PlayerUniqueNetID;
};

// *** CLASSES

class FHttpModule;

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UBaseGameInstance(const FObjectInitializer& ObjectInitializer);

protected:

	// ****************************************************************************************************************************************
	// ************************************ VARIABLES *****************************************************************************************
	// ****************************************************************************************************************************************

	// Main Menu ******************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_MainMenu_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Main Menu")
		UUserWidget* _UI_MainMenu_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_HostLobby_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Main Menu")
		UUserWidget* _UI_HostLobby_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_ClientLobby_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Main Menu")
		UUserWidget* _UI_ClientLobby_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_LobbyRoster_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Main Menu")
		UUserWidget* _UI_LobbyRoster_Instance = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Widgets | Main Menu")
		TSubclassOf<class UUserWidget> _UI_LoadingServer_Class;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Widgets | Main Menu")
		UUserWidget* _UI_LoadingServer_Instance = NULL;

	// Sessions *******************************************************************************************************************************

	/* Delegate called when session created */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;

	/* Delegate called when session started */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	/** Handles to registered delegates for creating/starting a session */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	/** Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	/** Handle to registered delegate for searching a session */
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	/** Delegate for joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	/** Handle to registered delegate for joining a session */
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	/** Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	/** Handle to registered delegate for destroying a session */
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	
	/** Delegate for receiving invite */
	//FOnSessionInviteReceivedDelegate OnSessionInviteReceivedDelegate;
	// 
	/** Handle to registered delegate for receiving an invite */
	//FDelegateHandle OnSessionInviteReceivedDelegateHandle;

	/** Delegate for accepting invite */
	FOnSessionUserInviteAcceptedDelegate OnSessionUserInviteAcceptedDelegate;

	/** Handle to registered delegate for accepting an invite */
	FDelegateHandle OnSessionUserInviteAcceptedDelegateHandle;

	/** Delegate for reading the friends list*/
	FOnReadFriendsListComplete FriendListReadCompleteDelegate;

	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	int32 _iMaxSessionPlayers = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
		FName _LobbyMapName;

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
		FName _MainMenuMap;

	// Matchmaking *****************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Matchmaking")
		UDataTable* _PlaylistDataTable;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Matchmaking")
		UDataTable* _GametypeDataTable;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Matchmaking")
		UDataTable* _MapDataTable;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Matchmaking")
		FPlaylistInfo _PlaylistInfo;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Matchmaking")
		FGameTypeInfo _GametypeInfo;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Matchmaking")
		FMapInfo _MapInfo;

	/*
	*
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Matchmaking")
		bool _bTeamBasedLobby = false;

	// Random Names ****************************************************************************************************************************

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Random Generator")
		TArray<FString> _RandPlayerNameList;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Random Generator")
		TArray<FString> _RandPlayerTagList;
	
private:

	// Sessions *******************************************************************************************************************************

	/**
	*  Function to host a game!
	*
	*  @Param    UserID      User that started the request
	*  @Param    SessionName    Name of the Session
	*  @Param	 ServerName		Name of the Server
	*  @Param    bIsLAN      Is this is LAN Game?
	*  @Param    bIsPresence    "Is the Session to create a presence Session"
	*  @Param    MaxNumPlayers          Number of Maximum allowed players on this "Session" (Server)
	*/
	bool HostSession(FUniqueNetIdRepl UserId, FName SessionName, FString ServerName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers, bool bIsPasswordProtected, FString SessionPassword);

	/**
	*  Function fired when a session create request has completed
	*
	*  @param SessionName the name of the session this callback is for
	*  @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	*  Function fired when a session start request has completed
	*
	*  @param SessionName the name of the session this callback is for
	*  @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	/**
	*  Find an online session
	*
	*  @param UserId user that initiated the request
	*  @param SessionName name of session this search will generate
	*  @param bIsLAN are we searching LAN matches
	*  @param bIsPresence are we searching presence sessions
	*/
	void FindSessions(FUniqueNetIdRepl UserId, FName SessionName, bool bIsLAN, bool bIsPresence);

	/**
	*  Delegate fired when a session search query has completed
	*
	*  @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnFindSessionsComplete(bool bWasSuccessful);

	/**
	*  Joins a session via a search result
	*
	*  @param SessionName name of session
	*  @param SearchResult Session to join
	*
	*  @return bool true if successful, false otherwise
	*/
	bool JoinASession(FUniqueNetIdRepl UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	/**
	*  Joins a session via a search result
	*
	*  @param SessionName name of session
	*  @param SearchResult Session to join
	*
	*  @return bool true if successful, false otherwise
	*/
	bool JoinASession(int32 LocalUserNum, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	/**
	*  Delegate fired when a session join request has completed
	*
	*  @param SessionName the name of the session this callback is for
	*  @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/**
	*  Delegate fired when a destroying an online session has completed
	*
	*  @param SessionName the name of the session this callback is for
	*  @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	* Delegate function fired when a session invite is accepted to join the session
	* 
	* @param    bWasSuccessful true if the async action completed without error, false if there was an error
	* @param	LocalUserNum	The Local user Number of the player who recived the invite
	* @param	InvitingPlayer	The inviting player
	* @param	TheSessionInvitedTo		the session invited to
	*/
	void OnSessionUserInviteAccepted(bool bWasSuccessful, int32 LocalUserNum, TSharedPtr<const FUniqueNetId> InvitingPlayer, const FOnlineSessionSearchResult& TheSessionInvitedTo);

	/**
	* Delegate fired when the friend list request has been processed
	* 
	* @param	LocalUserNum		The local user id (UniqueNetId) of the requesting player
	* @param	bWasSuccessful		true if the async action completed without error, false if there was an error
	* @param	ListName			the friend list name
	* @param	ErrorString			if there is any errors
	*/
	void OnReadFriendsListCompleted(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorString);

	FHttpModule* _fHttpModule;

	FString	_IPAddress;

	FString _APIUrl;

	FString _JSONKey;

	void DoRequest();

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

public:

	// ****************************************************************************************************************************************
	// ************************************ FUNCTIONS *****************************************************************************************
	// ****************************************************************************************************************************************

	// Startup ********************************************************************************************************************************

	virtual void Init() override;

	// Main Menu ******************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_MainMenu();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_HostLobby(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void HideUI_HostLobby();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_ClientLobby(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void HideUI_ClientLobby();

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_LobbyRoster(int ZOrder);

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowUI_LoadingServer(int ZOrder);

	// Random Names ****************************************************************************************************************************

	/*
	*
	*/
	UFUNCTION()
		TArray<FString> GetRandPlayerNameList() { return _RandPlayerNameList; }

	///////////////////////////////////////////////

	/*
	*
	*/
	UFUNCTION()
		TArray<FString> GetRandPlayerTagList() { return _RandPlayerTagList; }

	// Sessions *******************************************************************************************************************************
	
	// LAN player name to not use the Computer Name
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lan")
		FString _LanPlayerName;

	///////////////////////////////////////////////

	/**
	 *	Called from blueprints to host a session and start game
	 *	
	 *	@Param		ServerName				what name would appear to the players in the Server list
	 *	@Param		MaxNumPlayers			Max Number of Players who can join that session
	 *	@Param		bIsLan					Whether the session is LAN or no
	 *  @Param		bIsPresence				whether the session uses presence or no (default true)
	 *  @Param		bIsPasswordProtected	Whether the session is protected by a password or no (default false)
	 *	@Param		SessionPassword			The Password of the session if it is protected by a password (default empty string)
	 */
	UFUNCTION(BlueprintCallable, Category = "Network | Sessions")
		void StartOnlineGame(FString ServerName, int32 MaxNumPlayers, bool bIsLAN, bool bIsPresence = true, bool bIsPasswordProtected = false, FString SessionPassword = "");

	///////////////////////////////////////////////

	/**
	 *	Called from blueprints to find sessions then call and event to return the session results
	 *	
	 *	@Param		bIsLAN			Whether the session is LAN or no
	 *	@Param		bIsPresence		Whether the session uses presence or no
	 */
	UFUNCTION(BlueprintCallable, Category = "Network | Sessions")
		void FindOnlineGames(bool bIsLAN, bool bIsPresence);

	///////////////////////////////////////////////

	/**
	 *	Called from blueprints to join a session based on it's index in the returned session array
	 *	
	 *	@Param	 SessionIndex	 The index of the session in the Session Search Results array
	 */
	UFUNCTION(BlueprintCallable, Category = "Network | Sessions")
		void JoinOnlineGame(int32 SessionIndex);

	///////////////////////////////////////////////

	/**
	 *	Called from blueprints to destroy the session and leave game to main menu
	 */
	UFUNCTION(BlueprintCallable, Category = "Network | Sessions")
		void DestroySessionAndLeaveGame();

	///////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "Network | Friends")
		void SendSessionInviteToFriend(APlayerController* InvitingPlayer, const FBPUniqueNetId & Friend);

	///////////////////////////////////////////////

	/**
	 *	Called when the session search is complete to show the results in UMG
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Network | Sessions")
		void OnFoundSessionsCompleteUMG(const TArray<FCustomBlueprintSessionResult>& CustomSessionResults);

	///////////////////////////////////////////////

	/**
	*	Called to show an error message in UMG
	*	@Param	ErrorMessage The Error message you want to show
	*/
	UFUNCTION(BlueprintCallable, Category = "Network | Errors")
		void ShowErrorMessage(const FText & ErrorMessage);

	///////////////////////////////////////////////

	/**
	*	called to show an error message in UMG
	*	@Param	ErrorMessage The Error message you want to show
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Network | Errors")
		void ShowErrorMessageUMG(const FText & ErrorMessage);

	///////////////////////////////////////////////

	/**
	*	Check if steam is running
	*	@return		Whether the current online subsystem is steam or no
	*/
	UFUNCTION(BlueprintPure, Category = "Network | Friends")
		bool IsOnlineSubsystemSteam() const;

	///////////////////////////////////////////////

	/**
	 *	Called from the player state to get the player name
	 * 
	 *	@return		returns empty string if the player is on steam and returns the LanPlayerName if he is on LAN
	 */
	FString GetPlayerName() const;

	///////////////////////////////////////////////

	/**
	 *	Gets the current session
	 */
	IOnlineSessionPtr GetSession() const;

	///////////////////////////////////////////////

	/**
	 *	Gets the max number of players in the session
	 * 
	 *	@return		Max number of players in the session
	 */
	UFUNCTION(BlueprintPure, Category = "Network | Sessions")
		FORCEINLINE int32 GetSessionMaxPlayers() const { return _iMaxSessionPlayers; }

	///////////////////////////////////////////////

	/**
	 *	Gets the steam avatar of a player based on his UniqueNetId
	 * 
	 *	@Param		UniqueNetId		The UniqueNetId of the player you want to get his avatar
	 */
	UTexture2D* GetSteamAvatar(const FBPUniqueNetId UniqueNetId);

	///////////////////////////////////////////////

	/**
	 *	Called to get the list of steam friends a player has
	 *	
	 *	@Param		PlayerController		The player controller of the player asking for the friend list
	 *	@Param		FriendsList				List of friends' info in  blueprint wrapper structure
	 */
	UFUNCTION(BlueprintCallable, Category = "Network | Friends")
		void GetSteamFriendsList(APlayerController *PlayerController);

	///////////////////////////////////////////////

	/**
	 *	Called from the delegate when getting the friend list request in completed
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Network | Friends")
		void OnGetSteamFriendRequestCompleteUMG(const TArray<FSteamFriendInfo>& BPFriendsLists);

	///////////////////////////////////////////////

	/*
	*	Main function to call in order to get your public IP.
	*/
	UFUNCTION(BlueprintCallable, Category = "Network | Session")
		void GetMyPublicIP();

	/**
	 *	Returns an FString of the server's IP address.
	 */
	UFUNCTION(BlueprintCallable, Category = "Network | Session")
		FString GetCachedIP() const { return _IPAddress; }

	/*
	*	Subscribe to this event and get your IP address safely.
	*/
	UPROPERTY(BlueprintAssignable, Category = "Network | Session")
		FIPDelegate OnIPAddressReceived;

	///////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "Network | Travelling")
		void ServerTravel();

	UFUNCTION(BlueprintCallable, Category = "Network | Travelling")
		void ServerToGameplay();

	// Matchmaking *****************************************************************************************************************************

	UFUNCTION()
		UDataTable* GetPlaylistDataTable() { return _PlaylistDataTable; }

	UFUNCTION()
		UDataTable* GetGameTypeDataTable() { return _GametypeDataTable; }

	UFUNCTION()
		UDataTable* GetMapDataTable() { return _MapDataTable; }

	UFUNCTION()
		void SetGameTypeInfo(FGameTypeInfo GameTypeInfo) { _GametypeInfo = GameTypeInfo; }

	UFUNCTION()
		void SetMapInfo(FMapInfo MapInfo) { _MapInfo = MapInfo; }

	UFUNCTION(BlueprintPure)
		bool GetTeamBased() { return _bTeamBasedLobby; }

	UFUNCTION(BlueprintCallable)
		void SetTeamBased(bool TeamBased);

	UFUNCTION(BlueprintCallable)
		void GenerateRandomGamemode();

	UFUNCTION(BlueprintCallable)
		void GenerateRandomMap();

};
