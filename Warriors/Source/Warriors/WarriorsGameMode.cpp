// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarriorsGameMode.h"
#include "WarriorsCharacter.h"
#include "UObject/ConstructorHelpers.h"

AWarriorsGameMode::AWarriorsGameMode()
{
	// set default pawn class to our Blueprinted character
	///Game/Warriors/Characters/BP_Warriors
	////Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Warriors/Characters/BP_Warriors"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
