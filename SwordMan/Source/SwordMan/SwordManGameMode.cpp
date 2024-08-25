// Copyright Epic Games, Inc. All Rights Reserved.

#include "SwordManGameMode.h"
#include "SwordManCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASwordManGameMode::ASwordManGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
