// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "GameModeEditorStyle.h"

class FGameModeEditorCommands : public TCommands<FGameModeEditorCommands>
{
public:

	FGameModeEditorCommands()
		: TCommands<FGameModeEditorCommands>(TEXT("GameModeEditor"), NSLOCTEXT("Contexts", "GameModeEditor", "GameModeEditor Plugin"), NAME_None, FGameModeEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:

	TSharedPtr< FUICommandInfo > PluginAction;
	TSharedPtr< FUICommandInfo > RootGameType;
	TSharedPtr< FUICommandInfo > RootLoadout;
	TSharedPtr< FUICommandInfo > RootCharacter;

};
