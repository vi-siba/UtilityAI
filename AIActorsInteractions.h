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
    static AActor* GetClosestActor(TSubclassOf<AActor> ActorClassToFind, AActor* ExecuterActor);

    //////////////////////////////////
    ////////////ѕомен€ть на float или добавить возвращаемое bool+float
    //////////////////////////////////
    //UFUNCTION(BlueprintCallable, Category = "Check For Actors")
    //bool ObstaclesAlongWay(FVector Destination);

    UFUNCTION(BlueprintCallable, Category = "Check For Actors")
    static bool FindPath(const UObject* WorldContextObject, const FVector& Start, const FVector& End, TArray<FVector>& PathPoints);

protected:
private:

};
