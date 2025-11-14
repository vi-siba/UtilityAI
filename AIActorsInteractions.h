#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "AIActorsInteractions.generated.h"

/// <summary>
/// Class-component to storage reusable code for AI Controller/Actions/Abilities/ect.
/// </summary>
UCLASS(Blueprintable)
class UTILITYAI_API UAIActorsInteractions : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:

    /// <summary>
    /// Function to find closest actor of class ActorClassToFind to actor ExecuterActor
    /// </summary>
    UFUNCTION(BlueprintCallable, Category = "Check For Actors")
    static AActor* GetClosestActor(TSubclassOf<AActor> ActorClassToFind, AActor* ExecuterActor);

    //////////////////////////////////
    ////////////Ïîìåíÿòü íà float èëè äîáàâèòü âîçâðàùàåìîå bool+float
    //////////////////////////////////
    
    /// <summary>
    /// Function to check for obstacles along the NPC way from curretn ActionExecuterActor location to Destination, checking for Maximum Distance to Destination
    /// Maximum Distance to Destination can be used as attack range/skill-cast range and ect.
    /// </summary>
    /// <param name="ActionExecuterActor"></param>
    /// <param name="Destination"></param>
    /// <param name="MaxDistanceToDestination"></param>
    /// <returns></returns>
    UFUNCTION(BlueprintCallable, Category = "Check For Actors")
    static bool ObstaclesAlongWay(AActor* ActionExecuterActor, FVector Destination, float MaxDistanceToDestination);


    // WIP!
    // Add function to check for cooldown on an Ability and if so - return 0.0f utility, if not - continue;

protected:
private:

};
