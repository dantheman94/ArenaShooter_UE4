// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseGameInstance.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include <JsonObject.h>
#include <JsonReader.h>
#include <JsonSerializer.h>

// Startup ********************************************************************************************************************************

UBaseGameInstance::UBaseGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UBaseGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UBaseGameInstance::OnStartOnlineGameComplete);

	/** Bind function for FINDING a Session */
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UBaseGameInstance::OnFindSessionsComplete);


	/** Bind function for JOINING a Session */
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UBaseGameInstance::OnJoinSessionComplete);

	/** Bind function for DESTROYING a Session */
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UBaseGameInstance::OnDestroySessionComplete);


	/**Bind Function for receiving an invite*/
	//OnSessionInviteReceivedDelegate = FOnSessionInviteReceivedDelegate::CreateUObject(this, &UNWGameInstance::OnSessionInviteReceived);

	/**Bind Function for accepting an invite*/
	OnSessionUserInviteAcceptedDelegate = FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UBaseGameInstance::OnSessionUserInviteAccepted);

	/** Bind the function for completing the friend list request*/
	FriendListReadCompleteDelegate = FOnReadFriendsListComplete::CreateUObject(this, &UBaseGameInstance::OnReadFriendsListCompleted);

	_IPAddress = TEXT("");
	_APIUrl = TEXT("https://api.ipify.org/?format=json");
	_JSONKey = TEXT("api");
}

///////////////////////////////////////////////

void UBaseGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the Session Interface, so we can bind the accept delegate to it
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// OnSessionInviteReceivedDelegateHandle = Sessions->AddOnSessionInviteReceivedDelegate_Handle(OnSessionInviteReceivedDelegate);
			// we bind the delegate for accepting an invite to the session interface so when you accept an invite, you can join the game.
			OnSessionUserInviteAcceptedDelegateHandle = Sessions->AddOnSessionUserInviteAcceptedDelegate_Handle(OnSessionUserInviteAcceptedDelegate);
		}
	}
}

// Main Menu ******************************************************************************************************************************

/*
*
*/
void UBaseGameInstance::ShowUI_MainMenu_Implementation() {}

///////////////////////////////////////////////

/*
*
*/
void UBaseGameInstance::ShowUI_HostLobby_Implementation(int ZOrder = 0) {}

///////////////////////////////////////////////

/*
*
*/
void UBaseGameInstance::ShowUI_ClientLobby_Implementation(int ZOrder) {}

///////////////////////////////////////////////

/*
*
*/
void UBaseGameInstance::ShowUI_LobbyRoster_Implementation(int ZOrder = 0) {}

///////////////////////////////////////////////

/*
*
*/
void UBaseGameInstance::ShowUI_LoadingServer_Implementation(int ZOrder = 0) {}

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
bool UBaseGameInstance::HostSession(FUniqueNetIdRepl UserId, FName SessionName, FString ServerName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers, bool bIsPasswordProtected, FString SessionPassword)
{
	// Get the Online Subsystem to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the Session Interface, so we can call the "CreateSession" function on it
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid() && UserId.IsValid())
		{
			/*
				Fill in all the Session Settings that we want to use.
				There are more with SessionSettings.Set(...);
				For example the Map or the GameMode/Type.
			*/
			SessionSettings = MakeShareable(new FOnlineSessionSettings());
			SessionSettings->bIsLANMatch = bIsLAN;
			SessionSettings->bUsesPresence = bIsPresence;
			SessionSettings->NumPublicConnections = MaxNumPlayers;
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
			_iMaxSessionPlayers = MaxNumPlayers;

			// Setting a value in the FOnlineSessionSetting 's settings array
			SessionSettings->Set(SETTING_MAPNAME, _LobbyMapName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);

			// Making a temporary FOnlineSessionSetting variable to hold the data we want to add to the FOnlineSessionSetting 's settings array
			FOnlineSessionSetting ExtraSessionSetting;
			ExtraSessionSetting.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;

			// Setting the temporary data to the ServerName we got from UMG
			ExtraSessionSetting.Data = ServerName;

			// Adding the Server Name value in the FOnlineSessionSetting 's settings array using the key defined in header
			// the key can be any FNAME but we define it to avoid mistakes
			SessionSettings->Settings.Add(SETTING_SERVER_NAME, ExtraSessionSetting);

			// Setting the temporary data to the bIsPasswordProtected we got from UMG
			ExtraSessionSetting.Data = bIsPasswordProtected;

			// Adding the bIsPasswordProtected value in the FOnlineSessionSetting 's settings array using the key defined in header
			SessionSettings->Settings.Add(SETTING_SERVER_IS_PROTECTED, ExtraSessionSetting);

			// Setting the temporary data to the Password we got from UMG
			ExtraSessionSetting.Data = SessionPassword;

			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, *ExtraSessionSetting.Data.ToString());
			 
			// Adding the Password value in the FOnlineSessionSetting 's settings array using the key defined in header
			SessionSettings->Settings.Add(SETTING_SERVER_PROTECT_PASSWORD, ExtraSessionSetting);
					   
			// Set the delegate to the Handle of the SessionInterface
			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

			// Our delegate should get called when this is complete (doesn't need to be successful!)
			return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
	}
	return false;
}

///////////////////////////////////////////////

/**
*  Function fired when a session create request has completed
*
*  @param SessionName the name of the session this callback is for
*  @param bWasSuccessful true if the async action completed without error, false if there was an error
*/
void UBaseGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	 GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the OnlineSubsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub != NULL)
	{
		// Get the Session Interface to call the StartSession function
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the SessionComplete delegate handle, since we finished this call
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				// Set the StartSession delegate handle
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

				// Our StartSessionComplete delegate should get called after this
				Sessions->StartSession(SessionName);
			}
		}
	}
}

///////////////////////////////////////////////

/**
*  Function fired when a session start request has completed
*
*  @param SessionName the name of the session this callback is for
*  @param bWasSuccessful true if the async action completed without error, false if there was an error
*/
void UBaseGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the Online Subsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the Session Interface to clear the Delegate
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid()) { Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle); }
	}

	// If the start was successful, we can open a NewMap if we want. Make sure to use "listen" as a parameter!
	if (bWasSuccessful) { UGameplayStatics::OpenLevel(GetWorld(), _LobbyMapName, true, "listen"); }
}

///////////////////////////////////////////////

/**
*  Find an online session
*
*  @param UserId user that initiated the request
*  @param SessionName name of session this search will generate
*  @param bIsLAN are we searching LAN matches
*  @param bIsPresence are we searching presence sessions
*/
void UBaseGameInstance::FindSessions(FUniqueNetIdRepl UserId, FName SessionName, bool bIsLAN, bool bIsPresence)
{
	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the SessionInterface from our OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid() && UserId.IsValid())
		{
			
			// Fill in all the SearchSettings, like if we are searching for a LAN game and how many results we want to have
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->bIsLanQuery = bIsLAN;
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->PingBucketSize = 50;

			// We only want to set this Query Setting if "bIsPresence" is true
			if (bIsPresence) { SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals); }

			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

			// Set the Delegate to the Delegate Handle of the FindSession function
			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

			// Finally call the SessionInterface function. The Delegate gets called once this is finished
			Sessions->FindSessions(*UserId, SearchSettingsRef);
		}
	}

	// If something goes wrong, just call the Delegate Function directly with "false".
	else { OnFindSessionsComplete(false); }
}

///////////////////////////////////////////////

/**
*  Delegate fired when a session search query has completed
*
*  @param bWasSuccessful true if the async action completed without error, false if there was an error
*/
void UBaseGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OFindSessionsComplete bSuccess: %d"), bWasSuccessful));
	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get SessionInterface of the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the Delegate handle, since we finished this call
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			// Just debugging the Number of Search results. Can be displayed in UMG or something later on
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num()));

			TArray<FCustomBlueprintSessionResult> CustomSessionResults;

			// If we have found at least 1 session, we just going to debug them. You could add them to a list of UMG Widgets, like it is done in the BP version!
			if (SessionSearch->SearchResults.Num() > 0)
			{
				//ULocalPlayer* const Player = GetFirstGamePlayer();

				// "SessionSearch->SearchResults" is an Array that contains all the information. You can access the Session in this and get a lot of information.
				// This can be customized later on with your own classes to add more information that can be set and displayed
				for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
				{

					//temporary Session result to hold our data for this loop
					FCustomBlueprintSessionResult TempCustomSeesionResult;

					//uncomment if you want the session name to always be the name of the owning player (Computer name on LAN and Steam name online)
					//TempCustomSeesionResult.SessionName = SessionSearch->SearchResults[SearchIdx].Session.OwningUserName;
					TempCustomSeesionResult._bIsLan = SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.bIsLANMatch;
					TempCustomSeesionResult._CurrentNumberOfPlayers = SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.NumPublicConnections - SessionSearch->SearchResults[SearchIdx].Session.NumOpenPublicConnections;
					TempCustomSeesionResult._MaxNumberOfPlayers = SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.NumPublicConnections;
					TempCustomSeesionResult._Ping = SessionSearch->SearchResults[SearchIdx].PingInMs;

					// Get the server name
					SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.Get(SETTING_SERVER_NAME, TempCustomSeesionResult._SessionName);
					
					// Get if the server is password protected
					SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.Get(SETTING_SERVER_IS_PROTECTED, TempCustomSeesionResult._bIsPasswordProtected);
					
					// Get the Password if the session is Password Protected
					if (TempCustomSeesionResult._bIsPasswordProtected)
						SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.Get(SETTING_SERVER_PROTECT_PASSWORD, TempCustomSeesionResult._SessionPassword);

					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *TempCustomSeesionResult._SessionPassword);
									   
					CustomSessionResults.Add(TempCustomSeesionResult);

				}
			}

			// Call UMG to show sessions after the search ends
			OnFoundSessionsCompleteUMG(CustomSessionResults);

		}
	}
}

///////////////////////////////////////////////

/**
*  Joins a session via a search result
*
*  @param SessionName name of session
*  @param SearchResult Session to join
*
*  @return bool true if successful, false otherwise
*/
bool UBaseGameInstance::JoinASession(FUniqueNetIdRepl UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	bool bSuccessful = false;

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub != NULL)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid() && UserId.IsValid())
		{
			// Set the Handle again
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
			// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
			bSuccessful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
		}
	}

	return bSuccessful;
}

///////////////////////////////////////////////

/**
*  Joins a session via a search result
*
*  @param SessionName name of session
*  @param SearchResult Session to join
*
*  @return bool true if successful, false otherwise
*/
bool UBaseGameInstance::JoinASession(int32 LocalUserNum, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	bool bSuccessful = false;

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub != NULL)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Set the Handle again
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
			// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
			bSuccessful = Sessions->JoinSession(LocalUserNum, SessionName, SearchResult);
		}
	}

	return bSuccessful;
}

///////////////////////////////////////////////

/**
*  Delegate fired when a session join request has completed
*
*  @param SessionName the name of the session this callback is for
*  @param bWasSuccessful true if the async action completed without error, false if there was an error
*/
void UBaseGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnJoinSessionComplete %s, %d"), *SessionName.ToString(), static_cast<int32>(Result)));

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub != NULL)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the Delegate again
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

			// Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map, this is something the Blueprint Node "Join Session" does automatically!
			APlayerController * const PlayerController = GetFirstLocalPlayerController();

			// We need a FString to use ClientTravel and we can let the SessionInterface contruct such a
			// string for us by giving him the SessionName and an empty String. We want to do this, because every OnlineSubsystem uses different TravelURLs
			FString TravelURL;
			if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
			{
				// Finally call the ClienTravel. If you want, you could print the TravelURL to see how it really looks like
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

///////////////////////////////////////////////

/**
*  Delegate fired when a destroying an online session has completed
*
*  @param SessionName the name of the session this callback is for
*  @param bWasSuccessful true if the async action completed without error, false if there was an error
*/
void UBaseGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnDestroySessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub != NULL)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the delegate
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

			// If it was successful, we just load another level (main menu???)
			if (bWasSuccessful) { UGameplayStatics::OpenLevel(GetWorld(), _MainMenuMap, true); }
		}
	}
}

///////////////////////////////////////////////

/**
* Delegate function fired when a session invite is accepted to join the session
*
* @param    bWasSuccessful true if the async action completed without error, false if there was an error
* @param	LocalUserNum	The Local user Number of the player who recived the invite
* @param	InvitingPlayer	The inviting player
* @param	TheSessionInvitedTo		the session invited to
*/
void UBaseGameInstance::OnSessionUserInviteAccepted(bool bWasSuccessful, int32 LocalUserNum, TSharedPtr<const FUniqueNetId> InvitingPlayer, const FOnlineSessionSearchResult& TheSessionInvitedTo)
{
	if (bWasSuccessful)
	{
		if (TheSessionInvitedTo.IsValid()) { JoinASession(LocalUserNum, GameSessionName, TheSessionInvitedTo); }
	}
}

///////////////////////////////////////////////

/**
* Delegate fired when the friend list request has been processed
*
* @param	LocalUserNum		The local user id (UniqueNetId) of the requesting player
* @param	bWasSuccessful		true if the async action completed without error, false if there was an error
* @param	ListName			the friend list name
* @param	ErrorString			if there is any errors
*/
void UBaseGameInstance::OnReadFriendsListCompleted(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorString)
{
	if (bWasSuccessful)
	{
		// Get the steam online subsystem
		IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get(FName("Steam"));

		// Check if the online subsystem is valid
		if (OnlineSub)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, TEXT("Found Steam Online"));
			// Get Friends interface
			IOnlineFriendsPtr FriendsInterface = OnlineSub->GetFriendsInterface();

			// If the Friends Interface is valid
			if (FriendsInterface.IsValid())
			{

				// Get a list on all online players and store them in the FriendList
				TArray< TSharedRef<FOnlineFriend> > FriendList;
				FriendsInterface->GetFriendsList(LocalUserNum, ListName/*EFriendsLists::ToString((EFriendsLists::Default)),*/, FriendList);

				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, *FString::Printf(TEXT("Number of friends found is: %d"), FriendList.Num()));

				// For each loop to convert the FOnlineFriend array into the custom BP struct
				TArray<FSteamFriendInfo> BPFriendsList;
				for (TSharedRef<FOnlineFriend> Friend : FriendList)
				{
					//temp FSteamFriendInfo variable to add to the array
					FSteamFriendInfo TempSteamFriendInfo;
					TempSteamFriendInfo.PlayerUniqueNetID.SetUniqueNetId(Friend->GetUserId());
					TempSteamFriendInfo.PlayerAvatar = GetSteamAvatar(TempSteamFriendInfo.PlayerUniqueNetID);
					TempSteamFriendInfo.PlayerName = Friend->GetDisplayName();
					BPFriendsList.Add(TempSteamFriendInfo);
				}

				// Call blueprint to show the info on UMG
				OnGetSteamFriendRequestCompleteUMG(BPFriendsList);
			}
		}
	}
	else { ShowErrorMessageUMG(FText::FromString(ErrorString)); }
}

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
void UBaseGameInstance::StartOnlineGame(FString ServerName, int32 MaxNumPlayers, bool bIsLAN, bool bIsPresence /*= true*/, bool bIsPasswordProtected /*= false*/, FString SessionPassword /*= ""*/)
{
	// Creating a local player where we can get the UserID from
	ULocalPlayer* const Player = GetFirstGamePlayer();

	// Call our custom HostSession function. GameSessionName is a GameInstance variable
	HostSession(Player->GetPreferredUniqueNetId(), GameSessionName, ServerName, bIsLAN, bIsPresence, MaxNumPlayers, bIsPasswordProtected, SessionPassword);
}

///////////////////////////////////////////////

/**
 *	Called from blueprints to find sessions then call and event to return the session results
 *
 *	@Param		bIsLAN			Whether the session is LAN or no
 *	@Param		bIsPresence		Whether the session uses presence or no
 */
void UBaseGameInstance::FindOnlineGames(bool bIsLAN, bool bIsPresence)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();
	FindSessions(Player->GetPreferredUniqueNetId(), GameSessionName, bIsLAN, bIsPresence);
}

///////////////////////////////////////////////

/**
 *	Called from blueprints to join a session based on it's index in the returned session array
 *
 *	@Param	 SessionIndex	 The index of the session in the Session Search Results array
 */
void UBaseGameInstance::JoinOnlineGame(int32 SessionIndex)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	FOnlineSessionSearchResult SearchResult;
	SearchResult = SessionSearch->SearchResults[SessionIndex];

	_iMaxSessionPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;

	JoinASession(Player->GetPreferredUniqueNetId(), GameSessionName, SearchResult);
}

///////////////////////////////////////////////

/**
 *	Called from blueprints to destroy the session and leave game to main menu
 */
void UBaseGameInstance::DestroySessionAndLeaveGame()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
			Sessions->DestroySession(GameSessionName);
		}
	}
}

///////////////////////////////////////////////

/**
 *	Called when the session search is complete to show the results in UMG
 */
void UBaseGameInstance::SendSessionInviteToFriend(APlayerController* InvitingPlayer, const FBPUniqueNetId & Friend)
{
	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub != NULL)
	{
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(InvitingPlayer->GetLocalPlayer());
		if (LocalPlayer)
		{
			// Get SessionInterface from the OnlineSubsystem
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid() && LocalPlayer->GetPreferredUniqueNetId().IsValid() && Friend.IsValid())
			{
				// Send the invite
				Sessions->SendSessionInviteToFriend(*LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, *Friend.GetUniqueNetId());
			}
		}
	}
}

///////////////////////////////////////////////

/**
*	Called to show an error message in UMG
*	@Param	ErrorMessage The Error message you want to show
*/
void UBaseGameInstance::ShowErrorMessage(const FText & ErrorMessage)
{
	//Show the message on UMG through blueprints
	ShowErrorMessageUMG(ErrorMessage);
}

///////////////////////////////////////////////

/**
*	Check if steam is running
*	@return		Whether the current online subsystem is steam or no
*/
bool UBaseGameInstance::IsOnlineSubsystemSteam() const
{
	// Get the steam online subsystem
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get(FName("Steam"));

	if (OnlineSub) { return true; }
	else { return false; }
}

///////////////////////////////////////////////

/**
 *	Called from the player state to get the player name
 *
 *	@return		returns empty string if the player is on steam and returns the LanPlayerName if he is on LAN
 */
FString UBaseGameInstance::GetPlayerName() const
{
	// If steam is running, return an empty string
	if (IsOnlineSubsystemSteam()) { return ""; }

	// Else return the saved player LAN name
	else { return _LanPlayerName; }
}

///////////////////////////////////////////////

/**
 *	Gets the current session
 */
IOnlineSessionPtr UBaseGameInstance::GetSession() const
{
	// Get the Session Interface
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub) { return OnlineSub->GetSessionInterface(); }
	else { return nullptr; }
}

///////////////////////////////////////////////

/**
 *	Gets the steam avatar of a player based on his UniqueNetId
 *
 *	@Param		UniqueNetId		The UniqueNetId of the player you want to get his avatar
 */
UTexture2D* UBaseGameInstance::GetSteamAvatar(const FBPUniqueNetId UniqueNetId)
{
	if (UniqueNetId.IsValid())
	{
		/*
			uint32 Width = 0;
			uint32 Height = 0;

			//get the player iID
			uint64 id = *((uint64*)UniqueNetId.UniqueNetId->GetBytes());

			int Picture = 0;

			// get the Avatar ID using the player ID
			Picture = SteamFriends()->GetMediumFriendAvatar(id);

			//if the Avatar ID is not valid retrun null
			if (Picture == -1)
				return nullptr;

			//get the image size from steam
			SteamUtils()->GetImageSize(Picture, &Width, &Height);

			// if the image size is valid (most of this is from the Advanced Seesion Plugin as well, mordentral, THANK YOU!
			if (Width > 0 && Height > 0)
			{
				//Creating the buffer "oAvatarRGBA" and then filling it with the RGBA Stream from the Steam Avatar
				uint8 *oAvatarRGBA = new uint8[Width * Height * 4];
				
				//Filling the buffer with the RGBA Stream from the Steam Avatar and creating a UTextur2D to parse the RGBA Steam in
				SteamUtils()->GetImageRGBA(Picture, (uint8*)oAvatarRGBA, 4 * Height * Width * sizeof(char));

				UTexture2D* Avatar = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);

				// Switched to a Memcpy instead of byte by byte transer
				uint8* MipData = (uint8*)Avatar->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(MipData, (void*)oAvatarRGBA, Height * Width * 4);
				Avatar->PlatformData->Mips[0].BulkData.Unlock();

				// Original implementation was missing this!!
				// the hell man......
				// deallocating the memory used to get the avatar data
				delete[] oAvatarRGBA;

				//Setting some Parameters for the Texture and finally returning it
				Avatar->PlatformData->NumSlices = 1;
				Avatar->NeverStream = true;
				//Avatar->CompressionSettings = TC_EditorIcon;

				Avatar->UpdateResource();

				return Avatar;
			}
		*/
	}

	return nullptr;
}

///////////////////////////////////////////////

/**
 *	Called to get the list of steam friends a player has
 *
 *	@Param		PlayerController		The player controller of the player asking for the friend list
 *	@Param		FriendsList				List of friends' info in  blueprint wrapper structure
 */
void UBaseGameInstance::GetSteamFriendsList(APlayerController *PlayerController)
{
	// Check if the player controller is valid
	if (PlayerController != NULL)
	{
		// Get the steam online subsystem
		IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get(FName("Steam"));

		// Check if the online subsystem is valid
		if (OnlineSub != NULL)
		{			
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, TEXT("Found Steam Online"));

			// Get Friends interface
			IOnlineFriendsPtr FriendsInterface = OnlineSub->GetFriendsInterface();
			if (FriendsInterface.IsValid())
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Found friend interface"));

				// Get the local player from the player controller
				ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->Player);
				if (LocalPlayer != NULL)
				{
					// Read the friend list from the online subsystem then call the delegate when completed
					FriendsInterface->ReadFriendsList(LocalPlayer->GetControllerId(), EFriendsLists::ToString((EFriendsLists::InGamePlayers)), FriendListReadCompleteDelegate);
				}
			}
		}
	}
}

///////////////////////////////////////////////

void UBaseGameInstance::GetMyPublicIP()
{
	_IPAddress = TEXT("");
	_fHttpModule = &FHttpModule::Get();

	if (!_fHttpModule) { return; }

	DoRequest();
}

void UBaseGameInstance::DoRequest()
{
	TSharedRef< IHttpRequest > request = _fHttpModule->CreateRequest();
	request->OnProcessRequestComplete().BindUObject(this, &UBaseGameInstance::OnResponseReceived);
	request->SetURL(_APIUrl);
	request->SetVerb("GET");
	request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	request->SetHeader("Content-Type", TEXT("application/json"));
	request->ProcessRequest();
}

void UBaseGameInstance::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		TSharedPtr< FJsonObject > JsonObject;
		TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		if (FJsonSerializer::Deserialize(Reader, JsonObject)) { _IPAddress = JsonObject->GetStringField("ip"); }
	}

	OnIPAddressReceived.Broadcast(GetCachedIP());
}

///////////////////////////////////////////////

void UBaseGameInstance::ServerTravel()
{
	// Setup URL
	FString mapPath = "/Game/Levels/Debug/";
	FString mapName = "DebugTest";
	FString parameters = "?listen";

	// Perform travel
	GetWorld()->ServerTravel(mapPath + mapName + parameters, true);
}

void UBaseGameInstance::ServerToGameplay()
{
	// Setup URL
	FString mapPath = "/Game/Levels/Debug/";
	FString mapName = "DebugTest";
	FString parameters = "?listen";

	FURL url;
	url.Map = mapName;
	///url.AddOption("?listen");

	FString error = "";

	// Non-seamless travel to map
	FWorldContext context;
	context.SetCurrentWorld(GetWorld());	

	///UEngine::Browse(context, url, error);
}
