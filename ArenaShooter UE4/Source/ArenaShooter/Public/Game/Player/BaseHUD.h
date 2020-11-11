// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Containers/UnrealString.h"
#include "Structures.h"

#include "BaseHUD.generated.h"

// *** CLASSES

UCLASS()
class ARENASHOOTER_API ABaseHUD : public AHUD
{
	GENERATED_BODY()

protected:

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Display 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	*
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widgets | Display")
		bool _bDisplayHUD = true;

public:

	/*
	*
	*/
	virtual void DrawHUD() override;

	UFUNCTION(exec)
		virtual void Debug_DisplayHUD(bool Draw);

	UFUNCTION(BlueprintCallable)
		virtual void SetDisplayHud(bool Display);

	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;

};
