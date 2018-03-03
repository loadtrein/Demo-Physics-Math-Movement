// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "DemoGameMode.h"
#include "DemoHUD.h"
#include "DemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADemoGameMode::ADemoGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ADemoHUD::StaticClass();
}
