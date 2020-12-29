// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerInput.h"
#include "InputCoreTypes.h"

#include "Structures.generated.h"

class ABaseGameMode;
class ABasePlayerController;
class ABasePlayerState;
class AWeapon;

UENUM(BlueprintType)
enum class E_GameStates : uint8
{
	eGS_Menu UMETA(DisplayName = "Main Menu"),
	eGS_Ingame UMETA(DisplayName = "Gameplay")
};

UENUM(BlueprintType)
enum class E_TeamNames : uint8
{
	eTN_Spectator UMETA(DisplayName = "Unbiased Spectator"),
	eTN_Alpha UMETA(DisplayName = "Alpha"),
	eTN_Bravo UMETA(DisplayName = "Bravo"),
	eTN_Charlie UMETA(DisplayName = "Charlie"),
	eTN_Delta UMETA(DisplayName = "Delta")
};

UENUM(BlueprintType)
enum class E_GameTypes : uint8
{
	eGT_Undefined UMETA(DisplayName = "Undefined"),
	eGT_TDM UMETA(DisplayName = "Team Deathmatch"),
	eGT_CTF UMETA(DisplayName = "Capture the Flag"),
	eGT_KOTH UMETA(DisplayName = "King of the Hill"),
	eGT_Assault UMETA(DisplayName = "Bomb Assault"),
	EGT_Oddball UMETA(DisplayName = "Oddball"),
	EGT_Infection UMETA(DisplayName = "Infection")
};

UENUM(BlueprintType)
enum class E_MainMenuPage : uint8
{
	eGT_Splash UMETA(DisplayName = "Splash Screen"),
	eGT_Loading UMETA(DisplayName = "Loading Screen"),
	eGT_MainMenu UMETA(DisplayName = "Main Menu"),
	eGT_Matchmaking UMETA(DisplayName = "Matchmaking"),
	eGT_Playlist UMETA(DisplayName = "Playlist"),
	eGT_ServerBrowser UMETA(DisplayName = "Server Browser"),
	eGT_CreateMatch UMETA(DisplayName = "Host Match"),
	eGT_NotHosting UMETA(DisplayName = "Non-host Screen"),
	eGT_Searching UMETA(DisplayName = "Searching for Match"),
	eGT_PreMatch UMETA(DisplayName = "PreMatch"),
	eGT_BarracksHome UMETA(DisplayName = "Barracks Home"),
	eGT_SettingsHome UMETA(DisplayName = "Settings Home"),
	eGT_SettingsGamepad UMETA(DisplayName = "Settings Gamepad"),
	eGT_SettingsKeyBindings UMETA(DisplayName = "Settings Key Bindings"),
	eGT_SettingsDisplay UMETA(DisplayName = "Settings Display"),
	eGT_SettingsAudio UMETA(DisplayName = "Settings Audio")
};

UENUM(BlueprintType)
enum class E_ProfilePage : uint8
{
	ePP_ProfileHome UMETA(DisplayName = "Profile Home"),
	ePP_ServiceRecord UMETA(DisplayName = "Service Record"),
	ePP_Commendations UMETA(DisplayName = "Commendations"),
	ePP_Emblem UMETA(DisplayName = "Emblem"),
	ePP_Character UMETA(DisplayName = "Character"),
	ePP_WeaponSkins UMETA(DisplayName = "Weapon Skins"),
	ePP_SettingsHome UMETA(DisplayName = "Settings Home"),
	ePP_SettingsGamepad UMETA(DisplayName = "Settings Gamepad"),
	ePP_SettingsKeyBindings UMETA(DisplayName = "Settings Key Bindings"),
	ePP_SettingsDisplay UMETA(DisplayName = "Settings Display"),
	ePP_SettingsAudio UMETA(DisplayName = "Settings Audio")
};

UENUM(BlueprintType)
enum class E_CinematicType : uint8
{
	eGT_Opening UMETA(DisplayName = "Opening Cinematic"),
	eGT_Closing UMETA(DisplayName = "Closing Cinematic")
};

UENUM(BlueprintType)
enum class E_LoadoutType : uint8
{
	eLT_Primary UMETA(DisplayName = "Primary Weapon"),
	eLT_Reserve UMETA(DisplayName = "Reserve Weapon"),
	eLT_Grenade UMETA(DisplayName = "Grenades"),
	eLT_Equipment UMETA(DisplayName = "Equipment"),
};

UENUM(BlueprintType)
enum class E_ActionBinding : uint8
{
	eAB_ThrustAbility UMETA(DisplayName = "Thruster Ability"),
	eAB_CrouchSlide UMETA(DisplayName = "Crouch / Slide"),
	eAB_Jump UMETA(DisplayName = "Jump"),
	eAB_ClamberVault UMETA(DisplayName = "Clamber / Vault"),
	eAB_AimPrimary UMETA(DisplayName = "Aim Primary Weapon"),
	eAB_FirePrimary UMETA(DisplayName = "Fire Primary Weapon"),
	eAB_FireSecondary UMETA(DisplayName = "Fire Secondary Weapon"),
	eAB_ReloadPrimary UMETA(DisplayName = "Reload Primary Weapon"),
	eAB_ReloadSeccondary UMETA(DisplayName = "Reload Secondary Weapon"),
	eAB_TabWeapons UMETA(DisplayName = "Tab Inventory Weapons"),
	eAB_Grenade UMETA(DisplayName = "Grenade"),
	eAB_Interact UMETA(DisplayName = "Interact")
};

UENUM(BlueprintType)
enum class E_ItemType : uint8
{
	eWBT_Weapon UMETA(DisplayName = "Weapon"),
	eWBT_Scope UMETA(DisplayName = "Scope Attachment"),
	eWBT_Barrel UMETA(DisplayName = "Barrel Attachment"),
	eWBT_Accessory UMETA(DisplayName = "Accessory Attachment"),
	eWBT_Grenade UMETA(DisplayName = "Grenade"),
	eWBT_Equipment UMETA(DisplayName = "Equipment")
};

/*
*	Represents the different grenades
*/
UENUM(BlueprintType)
enum class E_GrenadeType : uint8
{
	eWBT_Frag UMETA(DisplayName = "Fragmentation Grenade"),
	eWBT_Smoke UMETA(DisplayName = "Electric-Smoke Grenade"),
	eWBT_Flame UMETA(DisplayName = "Incidenary Grenade"),
	eWBT_EMP UMETA(DisplayName = "EMP Grenade")
};

/*
*	Represents the different equipments
*/
UENUM(BlueprintType)
enum class E_EquipmentType : uint8
{
	eWBT_TripMine UMETA(DisplayName = "Trip Mine"),
	eWBT_GravityLift UMETA(DisplayName = "Gravity Lift"),
	eWBT_DeployableShield UMETA(DisplayName = "Deployable Shield"),
	eWBT_ADS UMETA(DisplayName = "Active Defence System"),
	eWBT_BubbleShield UMETA(DisplayName = "Bubble Shield"),
	eWBT_Hologram UMETA(DisplayName = "Hologram")
};

UENUM(BlueprintType)
enum class E_WeaponEngagementType : uint8
{
	eWBT_CQB UMETA(DisplayName = "Close Quarters Combat"),
	eWBT_Mid UMETA(DisplayName = "Mid-Range"),
	eWBT_Long UMETA(DisplayName = "Long Range")
};

/*
*	Represents a different WeaponBarrel attachments
*/
UENUM(BlueprintType)
enum class E_WeaponBarrelType : uint8
{
	eWBT_Silencer UMETA(DisplayName = "Silencer", ToolTip = "Removes radar ping from the weapon."),
	eWBT_Compensator UMETA(DisplayName = "Compensator", ToolTip = "Increases projectile accuracy when firing from the hip."),
	eWBT_MuzzleBreak UMETA(DisplayName = "Muzzle Break", ToolTip = "Reduces muzzle flash when firing the weapon."),
	eWBT_HeavyBarrel UMETA(DisplayName = "Heavy Barrel", ToolTip = "Decreases bullet drop.")
};

/*
*	Represents a different WeaponScope attachments
*/
UENUM(BlueprintType)
enum class E_WeaponScopeType : uint8
{
	eWST_Holographic UMETA(DisplayName = "Holo Sight", ToolTip = "Standard, 1x magnification sight."),
	eWST_COG UMETA(DisplayName = "COG Sight", ToolTip = "2x magnification sight that displays health info of the target."),
	eWST_Longshot UMETA(DisplayName = "Longshot Scope", ToolTip = "6x magnification scope."),
	eWST_Projection UMETA(DisplayName = "Projection Sight", ToolTip = "variable magnification sight that displays weapon info directly to the HUD."),
	eWST_Recon UMETA(DisplayName = "Recon Sight", ToolTip = "1.5x magnification sight that enables the user to see through smoke screens."),
	eWST_Sentinel UMETA(DisplayName = "Sentinel Scope", ToolTip = "4x - 8x multizoom scope."),
	eWST_ACOG UMETA(DisplayName = "ACOG Scope", ToolTip = "3x magnification scope.")
};

UENUM(BlueprintType)
enum class E_WeaponAccessoryType : uint8
{
	eWST_HeartbeatSensor UMETA(DisplayName = "Heartbeat Sensor", ToolTip = "Small display screen attachment that reveals targets in close proximity, infront of the player."),
	eWST_DuelWield UMETA(DisplayName = "Duel Wield", ToolTip = "Duplicates the weapon as a secondary for the player that they can wield at the same time in each hand."),
	eWST_FlashLight UMETA(DisplayName = "Flash Light", ToolTip = "Mounted flashlight whose vision cone will acutely blind targets."),
	eWST_GrenadeLauncher UMETA(DisplayName = "Grenade Launcher", ToolTip = "Undermounted grenade launcher."),
	eWST_ExtendedMags UMETA(DisplayName = "Extended Mags", ToolTip = "Increased magazine size."),
	eWST_LaserTargetter UMETA(DisplayName = "Laser", ToolTip = "Laser spotting device which will spot targets for a few seconds after being detected by the laser. (Triggered when aiming the weapon)."),
	eWST_VerticalGrip UMETA(DisplayName = "Vertical Grip", ToolTip = "Enhances vertical recoil of the weapon when aiming."),
	eWST_AngledGrip UMETA(DisplayName = "Angled Grip", ToolTip = "Enhances aiming / ADS speed.")
};

UENUM(BlueprintType)
enum class E_ItemStatsType : uint8
{
	eWST_Damage UMETA(DisplayName = "Damage"),
	eWST_Reload UMETA(DisplayName = "Reload Speed"),
	eWST_FireRate UMETA(DisplayName = "Firerate"),
	eWST_Accuracy UMETA(DisplayName = "Accuracy"),
	eWST_Recoil UMETA(DisplayName = "Recoil"),
	eWST_AmmoSize UMETA(DisplayName = "Ammo Size"),
	eWST_Range UMETA(DisplayName = "Effective Range"),
	eWST_ExplosionRadius UMETA(DisplayName = "Explosion Radius"),
	eWST_DetonationTime UMETA(DisplayName = "Detonation Time"),
	eWST_EffectTime UMETA(DisplayName = "Effect Time"),
};

/*
*	Represents a PlayerState's state within a session (an ingame match)
*/
UENUM(BlueprintType)
enum class E_SessionState : uint8
{
	eSS_Default UMETA(DisplayName = "Default"),
	eSS_Alive UMETA(DisplayName = "Alive"),
	eSS_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class E_MatchmakingState : uint8
{
	eMS_Idle UMETA(DisplayName = "Idle"),
	eMS_SearchingPlayers UMETA(DisplayName = "Searching for players"),
	eMS_ExpandingSearching UMETA(DisplayName = "Expanding search pool"),
	eMS_SearchComplete UMETA(DisplayName = "Players found"),
	eMS_Vetoing UMETA(DisplayName = "Confirming match")
};

USTRUCT(BlueprintType)
struct FPlaylistInfo : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FText _PlaylistName;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FText _PlaylistDescription;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		bool _bRankedPlaylist;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		int _iMaximumPlayers;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		int _iMinimumPlayers;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		int _iMaxLobbySize;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		int _iTeamSize;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		bool _bActivePlaylist;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		bool _bTimedPlaylist;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (EditCondition = "_bTimedPlaylist"))
		int _HourDuration;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<E_GameTypes> _GameTypes;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<FString> PlaylistMaps;
};

USTRUCT(BlueprintType)
struct FMapInfo : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FString MapName;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FString MapDescription;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FString MapLocation = "/Game/Levels/";

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		UTexture2D* MapThumbnail = NULL;
};

USTRUCT(BlueprintType)
struct FGameTypeInfo : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		E_GameTypes GameType;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TSubclassOf<ABaseGameMode> GameMode;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FString GameTypeName;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FString GameTypeDescription;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		UTexture2D* GametypeThumbnail = NULL;
};

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

		FPlayerInfo() {}

public:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		FString _PlayerName;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		FString _PlayerTag;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		bool _bIsHost = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		ABasePlayerState* _PlayerState = NULL;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		ABasePlayerController* _PlayerController = NULL;

	FString GetPlayerName() { return _PlayerName; }

	FString GetPlayerTag() { return _PlayerTag; }

	bool IsHost() { return _bIsHost; }

	ABasePlayerState* GetPlayerState() { return _PlayerState; }

	ABasePlayerController* GetPlayerController() { return _PlayerController; }

};

/*
*	Entry which represents a specific weapon animation montage and the origin transform that will
*	used when the animation montage is being played.
*/
USTRUCT(BlueprintType)
struct FRumbleInfo
{
	GENERATED_BODY()

		FRumbleInfo() {}

public:

	FRumbleInfo(float fIntensity, float fDuration, bool AffectsLargeLeftMotor, bool AffectsLargeRightMotor, bool AffectsSmallLeftMotor, bool AffectsSmallRightMotor)
	{
		Intensity = fIntensity;
		Duration = fDuration;
		bAffectsLargeLeftMotor = AffectsLargeLeftMotor;
		bAffectsLargeRightMotor = AffectsLargeRightMotor;
		bAffectsSmallLeftMotor = AffectsSmallLeftMotor;
		bAffectsSmallRightMotor = AffectsSmallRightMotor;
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Intensity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Duration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bAffectsLargeLeftMotor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bAffectsLargeRightMotor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bAffectsSmallLeftMotor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bAffectsSmallRightMotor;
};

/*
*	Entry which represents a specific weapon animation montage and the origin transform that will
*	used when the animation montage is being played.
*/
USTRUCT(BlueprintType)
struct FWeaponAnimation
{
	GENERATED_BODY()

		FWeaponAnimation() {}

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_FirstPickup = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FTransform _AnimationTransform;

};

USTRUCT(BlueprintType)
struct FAnimationListArms
{
	GENERATED_BODY()

		FAnimationListArms() {}

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_FirstPickup = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_Equip = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_Unequip = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_EquipDuelLeft = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_UnequipDuelLeft = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_EquipDuelRight = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_UnequipDuelRight = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_Inspect = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadFullNotEmpty = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadFullEmpty = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadDuelRightRaise = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadDuelRightLower = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadDuelLeftRaise = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadDuelLeftLower = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_FireProjectileHipfire = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_FireProjectileADS = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ProjectileMisfire = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_PumpProjectile = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_LoopingReloadEnter = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_LoopingReload = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_LoopingReloadExit = NULL;

};

USTRUCT(BlueprintType)
struct FAnimationListWeapon
{
	GENERATED_BODY()

		FAnimationListWeapon() {}

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_FirstPickup = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_Inspect = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadFullNotEmpty = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadFullEmpty = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadDuelRightRaise = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadDuelRightLower = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadDuelLeftRaise = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadDuelLeftLower = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_FireProjectileHipfire = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_FireProjectileADS = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ProjectileMisfire = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_PumpProjectile = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_LoopingReloadEnter = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_LoopingReload = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_LoopingReloadExit = NULL;

};

USTRUCT(BlueprintType)
struct FItemStats : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		E_ItemStatsType StatType;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FString StatText;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float StatPercent;
};

/*
*	DataTable structure which represents an entry for a Purchasable item
*	(used in the "BuyMenu")
*/
USTRUCT(BlueprintType)
struct FBuyItemInfo : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FString ItemName;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FString ItemDescription;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		int ItemCost;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		UTexture2D* ItemThumbnail;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		E_ItemType ItemType;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float ItemAwardModifer;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TSubclassOf<AActor> ActorClass;
};

/*
*	DataTable structure which represents an entry for a Weapon scope
*	(used in the "BuyMenu")
*/
USTRUCT(BlueprintType)
struct FWeaponScopeInfo : public FBuyItemInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		E_WeaponScopeType ScopeType;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<FItemStats> ItemStats;

	FWeaponScopeInfo()
	{
		ItemCost = 50;
		ItemType = E_ItemType::eWBT_Scope;
		ItemAwardModifer = 1.0f;
	}
};

/*
*	DataTable structure which represents an entry for a Weapon barrel
*	(used in the "BuyMenu")
*/
USTRUCT(BlueprintType)
struct FWeaponBarrelInfo : public FBuyItemInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		E_WeaponBarrelType BarrelType;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<FItemStats> ItemStats;

	FWeaponBarrelInfo()
	{
		ItemCost = 50;
		ItemType = E_ItemType::eWBT_Barrel;
		ItemAwardModifer = 1.0f;
	}
};

/*
*	DataTable structure which represents an entry for a Weapon accessory
*	(used in the "BuyMenu")
*/
USTRUCT(BlueprintType)
struct FWeaponAccessoryInfo : public FBuyItemInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		E_WeaponAccessoryType AccessoryType;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<FItemStats> ItemStats;

	FWeaponAccessoryInfo()
	{
		ItemCost = 50;
		ItemType = E_ItemType::eWBT_Accessory;
		ItemAwardModifer = 1.0f;
	}
};

/*
*	DataTable structure which represents an entry for a Weapon base
*	(used in the "BuyMenu")
*/
USTRUCT(BlueprintType)
struct FWeaponItemInfo : public FBuyItemInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		E_WeaponEngagementType WeaponEngagementType;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<E_WeaponScopeType> WeaponScopes;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<E_WeaponBarrelType> WeaponBarrels;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<E_WeaponAccessoryType> WeaponAccessories;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<FItemStats> ItemStats;

	FWeaponItemInfo()
	{
		ItemCost = 100;
		ItemType = E_ItemType::eWBT_Weapon;
		ItemAwardModifer = 1.0f;
	}
};

/*
*	DataTable structure which represents an entry for a Grenade
*	(used in the "BuyMenu")
*/
USTRUCT(BlueprintType)
struct FGrenadeItemInfo : public FBuyItemInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		E_GrenadeType GrenadeType;

	FGrenadeItemInfo()
	{
		ItemCost = 100;
		ItemType = E_ItemType::eWBT_Grenade;
		ItemAwardModifer = 1.0f;
	}
};

/*
*	DataTable structure which represents an entry for an Equipment
*	(used in the "BuyMenu")
*/
USTRUCT(BlueprintType)
struct FEquipmentItemInfo : public FBuyItemInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		E_EquipmentType EquipmentType;

	FEquipmentItemInfo()
	{
		ItemCost = 100;
		ItemType = E_ItemType::eWBT_Equipment;
		ItemAwardModifer = 1.0f;
	}
};

/*
*	DataTable structure which represents an entry in the LootPod pool
*/
USTRUCT(BlueprintType)
struct FLootPodItem : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
		int SpawnWeight = 50;
};
