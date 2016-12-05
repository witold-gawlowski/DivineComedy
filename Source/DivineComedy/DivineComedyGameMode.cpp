// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "DivineComedy.h"
#include "DivineComedyGameMode.h"
#include "DivineComedyHUD.h"
#include "DivineComedyCharacter.h"

ADivineComedyGameMode::ADivineComedyGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/DivineCharacter.DivineCharacter"));
  DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ADivineComedyHUD::StaticClass();
}
