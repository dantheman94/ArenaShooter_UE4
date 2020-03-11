// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"

#include "Structures.generated.h"

class ABaseGameMode;
class ABasePlayerController;
class ABasePlayerState;

// *** ENUMS

UENUM(BlueprintType)
enum class E_GameTypes : uint8
{
	eGT_TDM UMETA(DisplayName = "Team Deathmatch")
};

// *** STRUCTS

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
struct FPlaylistInfo : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		FString PlaylistName;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<E_GameTypes> PlaylistGameTypes;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TArray<FString> PlaylistMaps;
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
		UAnimMontage* _Anim_Inspect = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadFullNotEmpty = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadFullEmpty = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadDuelRight = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadDuelLeft = NULL;

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
		UAnimMontage* _Anim_ReloadDuelRight = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UAnimMontage* _Anim_ReloadDuelLeft = NULL;

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
