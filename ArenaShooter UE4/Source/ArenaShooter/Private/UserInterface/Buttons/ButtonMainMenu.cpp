// Fill out your copyright notice in the Description page of Project Settings.

#include "ButtonMainMenu.h"
#include "Internationalization/Text.h"
#include "Kismet/KismetStringLibrary.h"
#include "WidgetFooter.h"

/*
*	Called by both the game and the editor. Allows users to run initial setup for their widgets to better preview the setup in the designer and since generally that same setup code is required at runtime, it's called there as well.
*	WARNING** This is intended purely for cosmetic updates using locally owned data, you can not safely access any game related state, if you call something that doesn't expect to be run at editor time, you may crash the editor.
*	In the event you save the asset with blueprint code that causes a crash on evaluation. You can turn off PreConstruct evaluation in the Widget Designer settings in the Editor Preferences.
*/
void UButtonMainMenu::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (_CapitalizeButtonText)
		_ButtonText.ToUpper();
}

/*
*
*/
void UButtonMainMenu::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (!_IsBeingFocused)
		return;

	if (!_IsAcceptingNavigationInput)
	{
		_IsAcceptingNavigationInput = true;
		return;
	}

	// Up
	if (CheckForNavigation(_pNavigateUpWidget, _NavigateUpInput))
		return;

	// Down
	if (CheckForNavigation(_pNavigateDownWidget, _NavigateDownInput))
		return;

	// Left
	if (CheckForNavigation(_pNavigateLeftWidget, _NavigateLeftInput))
		return;

	// Right
	if (CheckForNavigation(_pNavigateRightWidget, _NavigateRightInput))
		return;
}

/*
*
*/
bool UButtonMainMenu::CheckForNavigation(UButtonMainMenu* TargetButton, TArray<FKey> InputKeys)
{
	bool ret = false;
	for (int i = 0; i < InputKeys.Num(); i++)
	{
		if (GetOwningPlayer()->WasInputKeyJustPressed(InputKeys[i]))
		{
			TargetButton->SetIsBeingFocused(true);
			ret = true;
			break;
		}
	}
	return ret;
}

/*
*
*/
void UButtonMainMenu::StartClick()
{
	if (_ClickedAnimation != NULL)
	{
		// We want the onClick functionality to perform after the animation is finished
		PlayAnimation(_ClickedAnimation);
		FTimerDelegate timerDelegate;
		timerDelegate.BindLambda([&] 
		{ 
			_OnClicked.Broadcast();
		});
		GetWorld()->GetTimerManager().ClearTimer(_fClickedAnimationHandle);
		GetWorld()->GetTimerManager().SetTimer(_fClickedAnimationHandle, timerDelegate, 1.0f, false, _ClickedAnimation->GetEndTime());
	}
	else // No OnClick animation exists for this widget so just do the onclick behaviours immediately
		_OnClicked.Broadcast();
}

/*
*
*/
void UButtonMainMenu::Hover()
{
	if (_IsBeingFocused)
		return;

	// Unfocus all sibling buttons then focus the new button
	for (int i = 0; i < _SiblingButtons.Num(); i++)
		_SiblingButtons[i]->SetIsBeingFocused(false);
	_IsBeingFocused = true;

	// Update button description in the footer?
	if (_FooterWidget != NULL)
	{
		if (_CapitalizeDescriptionText)
			_ButtonDescription.ToUpper();
		_FooterWidget->SetDescriptorText(_ButtonDescription);
	}

	// Hover feedback
	if (_HoverAnimation != NULL)
		PlayAnimation(_HoverAnimation, 0.0f, 1, EUMGSequencePlayMode::Type::Forward, 1.0f, true);
	_OnHovered.Broadcast();
}

/*
*
*/
void UButtonMainMenu::Unhover()
{
	_OnUnhovered.Broadcast();
}
