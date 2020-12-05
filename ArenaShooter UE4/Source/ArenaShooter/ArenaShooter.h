// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#if UE_BUILD_DEVELOPMENT

#define GAME_LOG_DEFINED 1

DECLARE_LOG_CATEGORY_EXTERN(GameLog, Log, All);

#define NETMODE_WORLD (((GEngine == nullptr) || (GetWorld() == nullptr)) ? TEXT("") \
        : (GEngine->GetNetMode(GetWorld()) == NM_Client) ? TEXT("[Client]") \
        : (GEngine->GetNetMode(GetWorld()) == NM_ListenServer) ? TEXT("[ListenServer]") \
        : (GEngine->GetNetMode(GetWorld()) == NM_DedicatedServer) ? TEXT("[DedicatedServer]") \
        : TEXT("[Standalone]"))

#if _MSC_VER
#define FUNC_NAME    TEXT(__FUNCTION__)
#else // FIXME - GCC?
#define FUNC_NAME    TEXT(__func__)
#endif

#define GAME_LOG(Message, ...) \
{ \
	const FString Msg = FString::Printf(TEXT(Message), ##__VA_ARGS__); \
	UE_LOG(GameLog, Log, TEXT("::::::: %s %s : %s :::::::"), NETMODE_WORLD, FUNC_NAME, *Msg); \
}

#define GAME_SCREENLOG(Message, ...) \
{ \
	const FString Msg = FString::Printf(TEXT(Message), ##__VA_ARGS__); \
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, Msg); \
}

#define GAME_WARN(Message, ...) \
{ \
	const FString Msg = FString::Printf(TEXT(Message), ##__VA_ARGS__); \
	UE_LOG(GameLog, Warning, TEXT("::::::: %s %s : %s :::::::"), NETMODE_WORLD, FUNC_NAME, *Msg); \
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, Msg); \
}

#define GAME_ERROR(Message, ...) \
{ \
	const FString Msg = FString::Printf(TEXT(Message), ##__VA_ARGS__); \
	UE_LOG(GameLog, Error, TEXT("::::::: %s %s : %s :::::::"), NETMODE_WORLD, FUNC_NAME, *Msg); \
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, Msg); \
}

#define GAME_HALT(Message, ...) \
{ \
	const FString Msg = FString::Printf(TEXT(Message), ##__VA_ARGS__); \
	UE_LOG(GameLog, Fatal, TEXT("====== %s %s : %s ======"), NETMODE_WORLD, FUNC_NAME, *Msg); \
}

#else
#define GAME_LOG(Message, ...)
#define GAME_SCREENLOG(Message, ...)
#define GAME_WARN(Message, ...)
#define GAME_ERROR(Message, ...)
#define GAME_HALT(Message, ...)
#endif