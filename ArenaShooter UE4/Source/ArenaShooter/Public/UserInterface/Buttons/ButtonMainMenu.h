// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateSignatureImpl.inl"

#include "ButtonMainMenu.generated.h"

// *** EVENT DISPATCHERS / DELEGATES

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHovered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnhovered);

// *** CLASSES

class UWidgetFooter;

/**
 * 
 */
UCLASS()
class ARENASHOOTER_API UButtonMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Navigation")
		UButtonMainMenu* _pNavigateUpWidget = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Navigation")
		TArray<FKey> _NavigateUpInput;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Navigation")
		UButtonMainMenu* _pNavigateDownWidget = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Navigation")
		TArray<FKey> _NavigateDownInput;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Navigation")
		UButtonMainMenu* _pNavigateLeftWidget = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Navigation")
		TArray<FKey> _NavigateLeftInput;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Navigation")
		UButtonMainMenu* _pNavigateRightWidget = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category="Navigation")
		TArray<FKey> _NavigateRightInput;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Navigation")
		bool _IsAcceptingNavigationInput = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Focus")
		bool _IsBeingFocused = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Focus")
		TArray<UButtonMainMenu*> _SiblingButtons;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Focus")
		bool _IsBeingClicked = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Focus")
		bool _Recursion = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Properties")
		bool _CapitalizeButtonText = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Properties")
		FText _ButtonText;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Properties")
		bool _CapitalizeDescriptionText = false;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Properties")
		FText _ButtonDescription;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Properties")
		UWidgetFooter* _FooterWidget = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Animation", Transient)
		UWidgetAnimation* _HoverAnimation = NULL;

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Animation", Transient)
		UWidgetAnimation* _ClickedAnimation = NULL;

	/*
	*	A timer handle used for referencing the clicked animation start delay.
	*/
	UPROPERTY()
		FTimerHandle _fClickedAnimationHandle;

	/*
	*
	*/
	UPROPERTY(BlueprintAssignable, EditAnywhere)
		FClicked _OnClicked;

	/*
	*
	*/
	UPROPERTY(BlueprintAssignable, EditAnywhere)
		FHovered _OnHovered;

	/*
	*
	*/
	UPROPERTY(BlueprintAssignable, EditAnywhere)
		FUnhovered _OnUnhovered;

public:

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		virtual void StartClick();

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		virtual void Hover();

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		virtual void Unhover();

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void SetIsBeingFocused(bool Focused) { _IsBeingFocused = Focused; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		ESlateVisibility GetDefaultVisibility() { return GetIsEnabled() && !_IsBeingFocused ? ESlateVisibility::Visible : ESlateVisibility::Hidden; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		ESlateVisibility GetHoveredVisibility() { return GetIsEnabled() && _IsBeingFocused ? ESlateVisibility::Visible : ESlateVisibility::Hidden; }

	/*
	*
	*/
	UFUNCTION(BlueprintPure)
		ESlateVisibility GetDisabledVisibility() { return !GetIsEnabled() ? ESlateVisibility::Visible : ESlateVisibility::Hidden; }

protected:

	/*
	*	Called by both the game and the editor. Allows users to run initial setup for their widgets to better preview the setup in the designer and since generally that same setup code is required at runtime, it's called there as well.
	*	WARNING** This is intended purely for cosmetic updates using locally owned data, you can not safely access any game related state, if you call something that doesn't expect to be run at editor time, you may crash the editor.
	*	In the event you save the asset with blueprint code that causes a crash on evaluation. You can turn off PreConstruct evaluation in the Widget Designer settings in the Editor Preferences.
	*/
	virtual void NativePreConstruct() override;

	/*
	*
	*/
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	/*
	*
	*/
	bool CheckForNavigation(UButtonMainMenu* TargetButton, TArray<FKey> InputKeys);

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void SetClickedAnimation(UWidgetAnimation* Animation) { _ClickedAnimation = Animation; }

	/*
	*
	*/
	UFUNCTION(BlueprintCallable)
		void SetHoverAnimation(UWidgetAnimation* Animation) { _HoverAnimation = Animation; }

};
