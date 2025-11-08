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
    ////////////Поменять на float или добавить возвращаемое bool+float
    //////////////////////////////////
    //UFUNCTION(BlueprintCallable, Category = "Check For Actors")
    //bool ObstaclesAlongWay(FVector Destination);






    UFUNCTION(BlueprintCallable, Category = "Pathfinding")
    static TArray<FVector> FindPath(
        UObject* WorldContextObject,
        const FVector& StartLocation,
        const FVector& TargetLocation,
        int32 GridWidth = 100,
        int32 GridHeight = 100,
        float CellSize = 100.0f
    );

private:
    // Вспомогательные функции
    static float CalculateHeuristic(const FIntPoint& From, const FIntPoint& To);
    static TArray<FIntPoint> GetNeighbors(const FIntPoint& Node, int32 GridWidth, int32 GridHeight);
    static TArray<FVector> ReconstructPath(const TMap<FIntPoint, FIntPoint>& CameFrom,
        const FIntPoint& Start, const FIntPoint& End,
        float CellSize);
    static bool IsPositionWalkable(UObject* WorldContextObject, const FIntPoint& Position, float CellSize);
    static FIntPoint WorldToGrid(const FVector& WorldLocation, float CellSize);
    static FVector GridToWorld(const FIntPoint& GridPosition, float CellSize);
    static bool IsValidGridPosition(const FIntPoint& GridPosition, int32 GridWidth, int32 GridHeight);


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

