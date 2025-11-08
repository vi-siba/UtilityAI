#include "AIActorsInteractions.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Engine/World.h"
#include "NavMesh/RecastNavMesh.h"

#include "AI/Navigation/NavQueryFilter.h"


AActor* UAIActorsInteractions::GetClosestActor(TSubclassOf<AActor> ActorClassToFind, AActor* ExecuterActor)
{
    if (!ActorClassToFind || !ExecuterActor)
        return nullptr;

    UWorld* World = ExecuterActor->GetWorld();
    if (!World)
        return nullptr;

    // Reset of basic parameters
    AActor* ClosestActor = nullptr;
    float MinDistance = FLT_MAX;

    for (TActorIterator<AActor> It(World, ActorClassToFind); It; ++It)
    {
        AActor* Actor = *It;
        if (Actor && Actor != ExecuterActor)
        {
            float Distance = FVector::Dist(Actor->GetActorLocation(), ExecuterActor->GetActorLocation());
            if (Distance < MinDistance)
            {
                MinDistance = Distance;
                ClosestActor = Actor;
            }
        }
    }

    return ClosestActor;
}

TArray<FVector> UAIActorsInteractions::FindPath(
    UObject* WorldContextObject,
    const FVector& StartLocation,
    const FVector& TargetLocation,
    int32 GridWidth,
    int32 GridHeight,
    float CellSize)
{
    TArray<FVector> Path;

    if (!WorldContextObject)
    {
        UE_LOG(LogTemp, Error, TEXT("FindPath: WorldContextObject is null!"));
        return Path;
    }

    FIntPoint StartGrid = WorldToGrid(StartLocation, CellSize);
    FIntPoint TargetGrid = WorldToGrid(TargetLocation, CellSize);

    // Проверка валидности точек
    if (!IsValidGridPosition(StartGrid, GridWidth, GridHeight) ||
        !IsValidGridPosition(TargetGrid, GridWidth, GridHeight))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid start or target position"));
        return Path;
    }

    // Если старт и цель одинаковы
    if (StartGrid == TargetGrid)
    {
        Path.Add(TargetLocation);
        return Path;
    }

    // Инициализация структур данных
    TMap<FIntPoint, FPathNode> Grid;
    TArray<FIntPoint> OpenSet;
    TMap<FIntPoint, FIntPoint> CameFrom;

    // Начальный узел
    FPathNode StartNode;
    StartNode.Position = StartGrid;
    StartNode.GCost = 0;
    StartNode.HCost = CalculateHeuristic(StartGrid, TargetGrid);
    StartNode.CalculateFCost();
    StartNode.bIsWalkable = IsPositionWalkable(WorldContextObject, StartGrid, CellSize);

    Grid.Add(StartGrid, StartNode);
    OpenSet.Add(StartGrid);

    // Основной цикл A*
    while (OpenSet.Num() > 0)
    {
        // Находим узел с минимальной F стоимостью
        int32 CurrentIndex = 0;
        FIntPoint CurrentNode = OpenSet[0];

        for (int32 i = 1; i < OpenSet.Num(); i++)
        {
            if (Grid.Contains(OpenSet[i]) && Grid.Contains(CurrentNode))
            {
                if (Grid[OpenSet[i]].FCost < Grid[CurrentNode].FCost)
                {
                    CurrentNode = OpenSet[i];
                    CurrentIndex = i;
                }
            }
        }

        // Проверка достижения цели
        if (CurrentNode == TargetGrid)
        {
            Path = ReconstructPath(CameFrom, StartGrid, TargetGrid, CellSize);

            // Визуализация финального пути
            UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
            if (World)
            {
                for (int32 i = 0; i < Path.Num() - 1; i++)
                {
                    DrawDebugLine(
                        World,
                        Path[i],
                        Path[i + 1],
                        FColor::Green,
                        false,
                        5.0f,
                        0,
                        10.0f
                    );

                    DrawDebugSphere(
                        World,
                        Path[i],
                        25.0f,
                        8,
                        FColor::Blue,
                        false,
                        5.0f
                    );
                }

                // Отметить старт и финиш
                DrawDebugSphere(World, StartLocation, 50.0f, 12, FColor::Green, false, 5.0f);
                DrawDebugSphere(World, TargetLocation, 50.0f, 12, FColor::Red, false, 5.0f);
            }

            return Path;
        }

        OpenSet.RemoveAt(CurrentIndex);

        // Обработка соседей
        TArray<FIntPoint> Neighbors = GetNeighbors(CurrentNode, GridWidth, GridHeight);

        for (const FIntPoint& Neighbor : Neighbors)
        {
            // Проверяем проходимость
            if (!IsPositionWalkable(WorldContextObject, Neighbor, CellSize))
                continue;

            // Создаем узел если его нет
            if (!Grid.Contains(Neighbor))
            {
                FPathNode NewNode;
                NewNode.Position = Neighbor;
                NewNode.bIsWalkable = IsPositionWalkable(WorldContextObject, Neighbor, CellSize);
                NewNode.HCost = CalculateHeuristic(Neighbor, TargetGrid);
                Grid.Add(Neighbor, NewNode);
            }

            FPathNode& NeighborNode = Grid[Neighbor];

            // Вычисляем временную G стоимость (расстояние между центрами ячеек)
            float TentativeGCost = Grid[CurrentNode].GCost + CellSize;

            if (TentativeGCost < NeighborNode.GCost)
            {
                CameFrom.Add(Neighbor, CurrentNode);
                NeighborNode.GCost = TentativeGCost;
                NeighborNode.CalculateFCost();

                if (!OpenSet.Contains(Neighbor))
                {
                    OpenSet.Add(Neighbor);
                }
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Path not found from (%d,%d) to (%d,%d)"),
        StartGrid.X, StartGrid.Y, TargetGrid.X, TargetGrid.Y);
    return Path;
}

// Проверка проходимости позиции
bool UAIActorsInteractions::IsPositionWalkable(UObject* WorldContextObject, const FIntPoint& Position, float CellSize)
{
    if (!WorldContextObject)
    {
        UE_LOG(LogTemp, Warning, TEXT("IsPositionWalkable: WorldContextObject is null"));
        return true;
    }

    FVector WorldPosition = GridToWorld(Position, CellSize);

    TArray<AActor*> ActorsToIgnore;
    TArray<FHitResult> HitResults;

    // Выполняем Sphere Trace для проверки препятствий
    bool bHit = UKismetSystemLibrary::SphereTraceMulti(
        WorldContextObject,
        WorldPosition - FVector(0, 0, 50),
        WorldPosition + FVector(0, 0, 50),
        CellSize * 0.4f, // 40% от размера ячейки
        UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
        true, // trace complex
        ActorsToIgnore,
        EDrawDebugTrace::None,
        HitResults,
        true
    );

    // Визуализация для отладки
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World)
    {
        FColor DebugColor = bHit ? FColor::Red : FColor::Green;
        float DebugDuration = 0.1f; // Короткая длительность чтобы не засорять экран

        DrawDebugSphere(
            World,
            WorldPosition,
            CellSize * 0.4f,
            8,
            DebugColor,
            false,
            DebugDuration
        );

        // Подпись для непроходимых ячеек
        if (bHit)
        {
            DrawDebugString(
                World,
                WorldPosition + FVector(0, 0, 100),
                FString::Printf(TEXT("(%d,%d)"), Position.X, Position.Y),
                nullptr,
                DebugColor,
                DebugDuration
            );
        }
    }

    return !bHit;
}

// Эвристическая функция (манхэттенское расстояние)
float UAIActorsInteractions::CalculateHeuristic(const FIntPoint& From, const FIntPoint& To)
{
    return FMath::Abs(From.X - To.X) + FMath::Abs(From.Y - To.Y);
}

// Получение соседних узлов (4-стороннее движение)
TArray<FIntPoint> UAIActorsInteractions::GetNeighbors(const FIntPoint& Node, int32 GridWidth, int32 GridHeight)
{
    TArray<FIntPoint> Neighbors;

    TArray<FIntPoint> Directions = {
        FIntPoint(1, 0),   // вправо
        FIntPoint(-1, 0),  // влево
        FIntPoint(0, 1),   // вперед
        FIntPoint(0, -1)   // назад
    };

    for (const FIntPoint& Direction : Directions)
    {
        FIntPoint Neighbor = Node + Direction;
        if (IsValidGridPosition(Neighbor, GridWidth, GridHeight))
        {
            Neighbors.Add(Neighbor);
        }
    }

    return Neighbors;
}

// Восстановление пути из карты родителей
TArray<FVector> UAIActorsInteractions::ReconstructPath(const TMap<FIntPoint, FIntPoint>& CameFrom,
    const FIntPoint& Start, const FIntPoint& End,
    float CellSize)
{
    TArray<FVector> Path;
    FIntPoint Current = End;

    // Восстанавливаем путь от конца к началу
    while (Current != Start)
    {
        Path.Add(GridToWorld(Current, CellSize));

        const FIntPoint* Parent = CameFrom.Find(Current);
        if (Parent)
        {
            Current = *Parent;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Path reconstruction failed - missing parent for (%d,%d)"), Current.X, Current.Y);
            break;
        }
    }

    // Добавляем стартовую позицию и переворачиваем
    Path.Add(GridToWorld(Start, CellSize));
    Algo::Reverse(Path);

    UE_LOG(LogTemp, Log, TEXT("Path found with %d points"), Path.Num());
    return Path;
}

// Конвертация мировых координат в координаты сетки
FIntPoint UAIActorsInteractions::WorldToGrid(const FVector& WorldLocation, float CellSize)
{
    return FIntPoint(
        FMath::RoundToInt(WorldLocation.X / CellSize),
        FMath::RoundToInt(WorldLocation.Y / CellSize)
    );
}

// Конвертация координат сетки в мировые координаты
FVector UAIActorsInteractions::GridToWorld(const FIntPoint& GridPosition, float CellSize)
{
    return FVector(
        GridPosition.X * CellSize + CellSize * 0.5f, // Центр ячейки
        GridPosition.Y * CellSize + CellSize * 0.5f,
        50.0f // Высота над землей
    );
}

// Проверка валидности позиции в сетке
bool UAIActorsInteractions::IsValidGridPosition(const FIntPoint& GridPosition, int32 GridWidth, int32 GridHeight)
{
    return GridPosition.X >= 0 && GridPosition.X < GridWidth &&
        GridPosition.Y >= 0 && GridPosition.Y < GridHeight;
}

/*
bool UAIActorsInteractions::ObstaclesAlongWay(FVector Destination)
{
    if (!ActionExecuterActor)
    {
        return true;
    }

    TArray<FVector> PathPoints;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys)
        return true;

    UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(GetWorld(), ExecuterActor->GetActorLocation(), Destination);
    if (NavPath && NavPath->IsValid())
    {
        PathPoints = NavPath->PathPoints;
    }

    int numberOfPoints = sizeof(PathPoints) / sizeof(PathPoints[0]);

    float Distance = FVector::Dist(PathPoints[numberOfPoints - 1], Destination);
    if (Distance < MinimumDistanceToTarget)
        return false;
    else
        return true;
}
*/