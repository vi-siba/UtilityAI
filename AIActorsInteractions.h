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
    //UFUNCTION(BlueprintCallable, Category = "Check For Actors")
    //bool ObstaclesAlongWay(FVector Destination);

    struct FPathNode3D
    {
        FIntPoint GridPosition;
        float Height;

        FPathNode3D(const FIntPoint& InGridPosition = FIntPoint(0, 0), float InHeight = 0.0f)
            : GridPosition(InGridPosition), Height(InHeight) {
        }

        // Определение оператора сравнения прямо в структуре
        bool operator==(const FPathNode3D& Other) const
        {
            return GridPosition == Other.GridPosition && FMath::IsNearlyEqual(Height, Other.Height, 1.0f);
        }

        friend uint32 GetTypeHash(const FPathNode3D& Node)
        {
            return HashCombine(GetTypeHash(Node.GridPosition), GetTypeHash(Node.Height));
        }
    };

   

    // 3D версии функций
    static TArray<FPathNode3D> GetNeighbors3D(const FPathNode3D& Node, float CellSize, float HeightAboveGround, UObject* WorldContextObject,
        AActor* ActorToIgnore);
    static  float CalculateDistance3D(const FPathNode3D& From, const FPathNode3D& To, float CellSize);
    static float CalculateHeuristic3D(const FPathNode3D& From, const FPathNode3D& To);
    static bool IsPositionWalkableAtSpecificHeight(UObject* WorldContextObject, const FIntPoint& Position, float Height, float CellSize, AActor* ActorToIgnore);
   static TArray<FVector> ReconstructPath3D(const TMap<FPathNode3D, FPathNode3D>& CameFrom,
        const FPathNode3D& Start, const FPathNode3D& End, float CellSize);

   static float GetSurfaceHeight(UObject* WorldContextObject, const FVector& WorldLocation, float CellSize, AActor* ActorToIgnore);



   UFUNCTION(BlueprintCallable, Category = "AI|Pathfinding")
   static TArray<FVector> FindPath(
       UObject* WorldContextObject,
       const FVector& StartLocation,
       const FVector& TargetLocation,
       float CellSize = 100.0f,
       float HeightAboveGround = 100.0f,
       AActor* CallingActor = nullptr);

private:

    // Проверка проходимости с учетом высоты над поверхностью
    static   bool IsPositionWalkableAtHeight(UObject* WorldContextObject, const FIntPoint& Position, float CellSize, float HeightAboveGround);

    // Поиск ближайшей проходимой позиции с учетом высоты
    static  FIntPoint FindNearestWalkablePositionAtHeight(UObject* WorldContextObject, const FIntPoint& StartPosition, float CellSize, float HeightAboveGround, int32 MaxRadius = 5);

    // Реконструкция пути с учетом высоты поверхности для каждой точки
    static  TArray<FVector> ReconstructPathWithSurfaceHeight(
        const TMap<FIntPoint, FIntPoint>& CameFrom,
        const FIntPoint& Start,
        const FIntPoint& End,
        float CellSize,
        float HeightAboveGround,
        UObject* WorldContextObject);

    // Расчет расстояния между узлами (для диагональных соседей)
    static  float CalculateDistance(const FIntPoint& From, const FIntPoint& To, float CellSize);



private:
    static float CalculatePathProgress(const FIntPoint& Start, const FIntPoint& End, const FIntPoint& Current);
    static TArray<FIntPoint> GetNeighbors(const FIntPoint& Node);

//        const FIntPoint& Start, const FIntPoint& End, float CellSize, float StartZ, float TargetZ);
    static FIntPoint WorldToGrid(const FVector& WorldLocation, float CellSize);
    static FVector GridToWorld(const FIntPoint& GridPosition, float CellSize);
    static bool IsValidGridPosition(const FIntPoint& GridPosition);

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
