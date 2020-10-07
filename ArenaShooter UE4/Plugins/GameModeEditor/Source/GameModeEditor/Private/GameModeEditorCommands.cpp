// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameModeEditorCommands.h"

#define LOCTEXT_NAMESPACE "FGameModeEditorModule"

void FGameModeEditorCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "GameModeEditor", "Execute GameModeEditor action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
