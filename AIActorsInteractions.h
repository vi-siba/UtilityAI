#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/GameplayStatics.h"
#include "AIBaseAction.generated.h"

/// <summary>
/// Class-component to storage reusable code for AI Controller/Actions/Abilities/ect.
/// </summary>
UCLASS(Blueprintable)
class UTILITYAI_API AIActorsInteractions
{
    GENERATED_BODY()

public:
    /// <summary>
    /// Function to find closest actor of class ActorClassToFind to actor ExecuterActor
    /// </summary>
    AActor* GetClosestActor(TSubclassOf<AActor> ActorClassToFind, AActor* ExecuterActor)

protected:
private:

};