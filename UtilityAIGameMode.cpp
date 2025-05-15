// Copyright Epic Games, Inc. All Rights Reserved.

#include "UtilityAIGameMode.h"
#include "UtilityAICharacter.h"
#include "UObject/ConstructorHelpers.h"

AUtilityAIGameMode::AUtilityAIGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
