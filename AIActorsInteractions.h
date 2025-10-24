#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/GameplayStatics.h"
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
    static AActor* GetClosestActor(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, AActor* ActionExecuter);


protected:
private:

};
