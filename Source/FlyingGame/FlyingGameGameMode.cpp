// Copyright Epic Games, Inc. All Rights Reserved.

#include "FlyingGameGameMode.h"
#include "FlyingGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFlyingGameGameMode::AFlyingGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
