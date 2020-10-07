// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameModeEditor.h"
#include "GameModeEditorStyle.h"
#include "GameModeEditorCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName GameModeEditorTabName("GameModeEditor");

#define LOCTEXT_NAMESPACE "FGameModeEditorModule"

void FGameModeEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FGameModeEditorStyle::Initialize();
	FGameModeEditorStyle::ReloadTextures();

	FGameModeEditorCommands::Register();
	
	_PluginCommands = MakeShareable(new FUICommandList);

	_PluginCommands->MapAction(
		FGameModeEditorCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FGameModeEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	///	_PluginCommands->MapAction(
	///		FGameModeEditorCommands::Get().RootGameType,
	///		FExecuteAction::CreateLambda([]() { OptionGameType(); }));
	///
	///	_PluginCommands->MapAction(
	///		FGameModeEditorCommands::Get().RootLoadout,
	///		FExecuteAction::CreateLambda([]() { OptionLoadout(); }));
	///
	///	_PluginCommands->MapAction(
	///		FGameModeEditorCommands::Get().RootCharacter,
	///		FExecuteAction::CreateLambda([]() { OptionCharacter(); }));

	_PluginCommands->MapAction(
		FGameModeEditorCommands::Get().RootGameType,
		FExecuteAction::CreateRaw(this, &FGameModeEditorModule::OptionGameType),
		FCanExecuteAction());

	_PluginCommands->MapAction(
		FGameModeEditorCommands::Get().RootLoadout,
		FExecuteAction::CreateRaw(this, &FGameModeEditorModule::OptionLoadout),
		FCanExecuteAction());

	_PluginCommands->MapAction(
		FGameModeEditorCommands::Get().RootCharacter,
		FExecuteAction::CreateRaw(this, &FGameModeEditorModule::OptionCharacter),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FGameModeEditorModule::RegisterMenus));
}

void FGameModeEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FGameModeEditorStyle::Shutdown();

	FGameModeEditorCommands::Unregister();
}

void FGameModeEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FGameModeEditorCommands::Get().PluginAction, _PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FGameModeEditorCommands::Get().PluginAction));
				Entry.SetCommandList(_PluginCommands);
			}
		}
	}
}

void FGameModeEditorModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FGameModeEditorModule::PluginButtonClicked()")),
							FText::FromString(TEXT("GameModeEditor.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FGameModeEditorModule::OptionGameType()
{
	UE_LOG(LogTemp, Warning, TEXT("Option GameType"));
}

void FGameModeEditorModule::OptionLoadout()
{
	UE_LOG(LogTemp, Warning, TEXT("Option Loadout"));
}

void FGameModeEditorModule::OptionCharacter()
{
	UE_LOG(LogTemp, Warning, TEXT("Option Character"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGameModeEditorModule, GameModeEditor)