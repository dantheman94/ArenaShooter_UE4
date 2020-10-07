// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FGameModeEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	void RegisterMenus();
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();

	/**  */
	void OptionGameType();

	/**  */
	void OptionLoadout();

	/**  */
	void OptionCharacter();

private:

	TSharedPtr<class FUICommandList> _PluginCommands;
	
};
