// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseHUD.h"

/*
*
*/
void ABaseHUD::DrawHUD()
{

}

void ABaseHUD::Debug_DisplayHUD(bool Draw)
{
	_bDisplayHUD = Draw;
}

void ABaseHUD::SetDisplayHud(bool Display)
{
	_bDisplayHUD = Display;
}

bool ABaseHUD::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool handled = Super::ProcessConsoleExec(Cmd, Ar, Executor);
	if (!handled)
	{
		handled &= this->ProcessConsoleExec(Cmd, Ar, Executor);
	}
	return handled;
}