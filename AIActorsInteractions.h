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
    ////////////Ïîìåíÿòü íà float èëè äîáàâèòü âîçâðàùàåìîå bool+float
    //////////////////////////////////
    //UFUNCTION(BlueprintCallable, Category = "Check For Actors")
    //bool ObstaclesAlongWay(FVector Destination);





    UFUNCTION(BlueprintCallable, Category = "Pathfinding")
    static TArray<FVector> FindPath(
        UObject* WorldContextObject,
        const FVector& StartLocation,
        const FVector& TargetLocation,
        float CellSize = 100.0f
    );

private:
    static float CalculatePathProgress(const FIntPoint& Start, const FIntPoint& End, const FIntPoint& Current);
    static TArray<FIntPoint> GetNeighbors(const FIntPoint& Node);
    static TArray<FVector> ReconstructPath(const TMap<FIntPoint, FIntPoint>& CameFrom,
        const FIntPoint& Start, const FIntPoint& End, float CellSize, float StartZ, float TargetZ);
    static bool IsPositionWalkable(UObject* WorldContextObject, const FIntPoint& Position, float CellSize);
    static FIntPoint WorldToGrid(const FVector& WorldLocation, float CellSize);
    static FVector GridToWorld(const FIntPoint& GridPosition, float CellSize);
    static bool IsValidGridPosition(const FIntPoint& GridPosition);

    static bool IsPositionWalkable(UObject* WorldContextObject, const FIntPoint& Position, float CellSize, float CheckHeight);
    static FIntPoint FindNearestWalkablePosition(UObject* WorldContextObject, const FIntPoint& StartPosition, float CellSize, float CheckHeight, int32 MaxRadius);

    static float CalculateHeuristic(const FIntPoint& From, const FIntPoint& To);

private:

protected:
private:

};




USTRUCT(BlueprintType)
struct FPathNode
{
    GENERATED_BODY()

    FIntPoint Position;
    float GCost = FLT_MAX;
    float HCost = 0;
    float FCost = FLT_MAX;
    FIntPoint Parent;
    bool bIsWalkable = true;

    void CalculateFCost() { FCost = GCost + HCost; }

    bool operator<(const FPathNode& Other) const { return FCost < Other.FCost; }
};
