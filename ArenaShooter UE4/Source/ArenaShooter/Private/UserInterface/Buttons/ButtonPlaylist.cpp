// Fill out your copyright notice in the Description page of Project Settings.

#include "ButtonPlaylist.h"

	/*
	*
	*/
void UButtonPlaylist::Setup(FPlaylistInfo ButtonInfo, TArray<UButtonPlaylist*> FriendlyButtons)
{
	// Setup the button info
	_ButtonInfo = ButtonInfo;
	_ButtonText = ButtonInfo._PlaylistName;

	// The first button in the array should be the focused button
	_IsBeingFocused = FriendlyButtons[0] == this;

	// Friendly buttons
	FriendlyButtons.Remove(this);
	for (int i = 0; i < FriendlyButtons.Num(); i++)
		_SiblingButtons.Add(FriendlyButtons[i]);
	
	_OnClicked.BindLambda([]
	{

	});
}

/*
*
*/
void UButtonPlaylist::Hover()
{
	Super::Hover();

	AHUDMainMenu* hud = Cast<AHUDMainMenu>(GetOwningPlayer()->GetHUD());
	hud->GetPlaylistPreviewWidget()->SetPlaylistInfo(_ButtonInfo)
}
