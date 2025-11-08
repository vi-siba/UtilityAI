#include "AIActorsInteractions.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Engine/World.h"
#include "NavMesh/RecastNavMesh.h"

#include "AI/Navigation/NavQueryFilter.h"

TArray<FVector> UAIActorsInteractions::FindPath(
    UObject* WorldContextObject,
    const FVector& StartLocation,
    const FVector& TargetLocation,
    float CellSize)
{
    TArray<FVector> Path;

    if (!WorldContextObject)
    {
        UE_LOG(LogTemp, Error, TEXT("FindPath: WorldContextObject is null!"));
        return Path;
    }

    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("FindPath: Cannot get World!"));
        return Path;
    }

    // Сохраняем исходные высоты старта и цели
    float StartZ = StartLocation.Z;
    float TargetZ = TargetLocation.Z;

    FIntPoint StartGrid = WorldToGrid(StartLocation, CellSize);
    FIntPoint TargetGrid = WorldToGrid(TargetLocation, CellSize);

    UE_LOG(LogTemp, Log, TEXT("FindPath: StartGrid (%d, %d), TargetGrid (%d, %d)"),
        StartGrid.X, StartGrid.Y, TargetGrid.X, TargetGrid.Y);

    // Визуализация стартовой и конечной точек
    if (World)
    {
        FVector StartWorldPos = GridToWorld(StartGrid, CellSize);
        FVector TargetWorldPos = GridToWorld(TargetGrid, CellSize);
        StartWorldPos.Z = StartZ;
        TargetWorldPos.Z = TargetZ;

        DrawDebugSphere(World, StartWorldPos, 50.0f, 12, FColor::Green, false, 10.0f);
        DrawDebugSphere(World, TargetWorldPos, 50.0f, 12, FColor::Red, false, 10.0f);
        DrawDebugLine(World, StartWorldPos, TargetWorldPos, FColor::Yellow, false, 10.0f, 0, 2.0f);
    }

    if (StartGrid == TargetGrid)
    {
        UE_LOG(LogTemp, Log, TEXT("Start and target are the same grid position"));
        Path.Add(TargetLocation);
        return Path;
    }

    // Проверяем доступность стартовой и конечной точек с отладкой
    bool bStartWalkable = IsPositionWalkable(WorldContextObject, StartGrid, CellSize, StartZ);
    bool bTargetWalkable = IsPositionWalkable(WorldContextObject, TargetGrid, CellSize, TargetZ);

    UE_LOG(LogTemp, Log, TEXT("Start walkable: %s, Target walkable: %s"),
        bStartWalkable ? TEXT("True") : TEXT("False"),
        bTargetWalkable ? TEXT("True") : TEXT("False"));

    if (!bStartWalkable)
    {
        UE_LOG(LogTemp, Warning, TEXT("Start position is not walkable! Searching for nearest walkable start..."));

        // Пытаемся найти ближайшую проходимую точку от старта
        FIntPoint WalkableStart = FindNearestWalkablePosition(WorldContextObject, StartGrid, CellSize, StartZ, 5);
        if (WalkableStart != StartGrid)
        {
            UE_LOG(LogTemp, Log, TEXT("Found walkable start at (%d, %d)"), WalkableStart.X, WalkableStart.Y);
            StartGrid = WalkableStart;
            bStartWalkable = true;
        }
    }

    if (!bTargetWalkable)
    {
        UE_LOG(LogTemp, Warning, TEXT("Target position is not walkable! Searching for nearest walkable target..."));

        // Пытаемся найти ближайшую проходимую точку от цели
        FIntPoint WalkableTarget = FindNearestWalkablePosition(WorldContextObject, TargetGrid, CellSize, TargetZ, 5);
        if (WalkableTarget != TargetGrid)
        {
            UE_LOG(LogTemp, Log, TEXT("Found walkable target at (%d, %d)"), WalkableTarget.X, WalkableTarget.Y);
            TargetGrid = WalkableTarget;
            bTargetWalkable = true;
        }
    }

    if (!bStartWalkable || !bTargetWalkable)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot find walkable start or target position"));
        return Path;
    }

    // Упрощенные структуры данных
    TMap<FIntPoint, float> GCostMap;
    TArray<FIntPoint> OpenSet;
    TMap<FIntPoint, FIntPoint> CameFrom;

    GCostMap.Add(StartGrid, 0.0f);
    OpenSet.Add(StartGrid);

    int32 IterationCount = 0;
    const int32 MaxIterations = 10000;

    while (OpenSet.Num() > 0 && IterationCount < MaxIterations)
    {
        IterationCount++;

        // Находим узел с минимальной F стоимостью
        int32 CurrentIndex = 0;
        FIntPoint CurrentNode = OpenSet[0];
        float CurrentFCost = GCostMap.FindRef(CurrentNode) + CalculateHeuristic(CurrentNode, TargetGrid);

        for (int32 i = 1; i < OpenSet.Num(); i++)
        {
            FIntPoint TestNode = OpenSet[i];
            float TestFCost = GCostMap.FindRef(TestNode) + CalculateHeuristic(TestNode, TargetGrid);

            if (TestFCost < CurrentFCost)
            {
                CurrentNode = TestNode;
                CurrentIndex = i;
                CurrentFCost = TestFCost;
            }
        }

        if (CurrentNode == TargetGrid)
        {
            UE_LOG(LogTemp, Log, TEXT("Path found! Iterations: %d, Path length: %d nodes"),
                IterationCount, CameFrom.Num() + 1);
            return ReconstructPath(CameFrom, StartGrid, TargetGrid, CellSize, StartZ, TargetZ);
        }

        OpenSet.RemoveAt(CurrentIndex);

        TArray<FIntPoint> Neighbors = GetNeighbors(CurrentNode);

        for (const FIntPoint& Neighbor : Neighbors)
        {
            // Для целевой точки пропускаем проверку проходимости (уже проверили выше)
            if (Neighbor != TargetGrid && !IsPositionWalkable(WorldContextObject, Neighbor, CellSize, StartZ))
            {
                continue;
            }

            float TentativeGCost = GCostMap.FindRef(CurrentNode) + CellSize;
            float NeighborGCost = GCostMap.FindRef(Neighbor, MAX_FLT);

            if (TentativeGCost < NeighborGCost)
            {
                CameFrom.Add(Neighbor, CurrentNode);
                GCostMap.Add(Neighbor, TentativeGCost);

                if (!OpenSet.Contains(Neighbor))
                {
                    OpenSet.Add(Neighbor);
                }
            }
        }
    }

    if (IterationCount >= MaxIterations)
    {
        UE_LOG(LogTemp, Warning, TEXT("Path finding exceeded maximum iterations (%d)"), MaxIterations);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Path not found after %d iterations. OpenSet empty."), IterationCount);
    }

    return Path;
}

bool UAIActorsInteractions::IsPositionWalkable(UObject* WorldContextObject, const FIntPoint& Position, float CellSize, float CheckHeight)
{
    if (!WorldContextObject) return true;

    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World) return true;

    FVector WorldPosition = GridToWorld(Position, CellSize);
    WorldPosition.Z = CheckHeight; // Используем переданную высоту

    // Настраиваем параметры трассировки в зависимости от размера ячейки
    float TraceRadius = FMath::Max(CellSize * 0.3f, 10.0f);
    float TraceHeight = FMath::Max(CellSize * 0.5f, 50.0f);

    FVector TraceStart = WorldPosition + FVector(0, 0, TraceHeight);
    FVector TraceEnd = WorldPosition - FVector(0, 0, TraceHeight);

    TArray<AActor*> ActorsToIgnore;
    TArray<FHitResult> HitResults;

    // Трассируем только по статическим объектам
    bool bHasObstacle = UKismetSystemLibrary::SphereTraceMulti(
        World,
        TraceStart,
        TraceEnd,
        TraceRadius,
        UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
        false, // Не использовать сложные коллизии
        ActorsToIgnore,
        EDrawDebugTrace::None, // Можно включить для отладки: EDrawDebugTrace::ForDuration
        HitResults,
        true
    );

    // Анализируем результаты трассировки
    if (bHasObstacle)
    {
        for (const FHitResult& Hit : HitResults)
        {
            // Игнорируем триггеры и определенные типы объектов
            UPrimitiveComponent* HitComponent = Hit.GetComponent();
            if (HitComponent && (HitComponent->IsA<UStaticMeshComponent>() || HitComponent->Mobility == EComponentMobility::Static))
            {
                // Проверяем, не является ли это полом/землей под нами
                bool bIsFloor = (Hit.ImpactNormal.Z > 0.7f); // Нормаль смотрит вверх - это пол

                if (!bIsFloor)
                {
                    // Это препятствие (стена, объект и т.д.)
                    UE_LOG(LogTemp, VeryVerbose, TEXT("Obstacle found at (%d, %d): %s"),
                        Position.X, Position.Y, *Hit.GetActor()->GetName());
                    return false;
                }
            }
        }

        // Если дошли сюда, значит все попадания - это пол/земля
        return true;
    }

    // Если нет попаданий, точка проходима
    return true;
}

FIntPoint UAIActorsInteractions::FindNearestWalkablePosition(UObject* WorldContextObject, const FIntPoint& StartPosition, float CellSize, float CheckHeight, int32 MaxRadius)
{
    if (IsPositionWalkable(WorldContextObject, StartPosition, CellSize, CheckHeight))
    {
        return StartPosition;
    }

    // Поиск по спирали от стартовой позиции
    for (int32 Radius = 1; Radius <= MaxRadius; Radius++)
    {
        for (int32 X = -Radius; X <= Radius; X++)
        {
            for (int32 Y = -Radius; Y <= Radius; Y++)
            {
                if (FMath::Abs(X) == Radius || FMath::Abs(Y) == Radius)
                {
                    FIntPoint TestPosition = StartPosition + FIntPoint(X, Y);
                    if (IsPositionWalkable(WorldContextObject, TestPosition, CellSize, CheckHeight))
                    {
                        return TestPosition;
                    }
                }
            }
        }
    }

    return StartPosition; // Возвращаем оригинал, если не нашли ничего
}

float UAIActorsInteractions::CalculateHeuristic(const FIntPoint& From, const FIntPoint& To)
{
    // Манхэттенское расстояние для 2D
    return FMath::Abs(From.X - To.X) + FMath::Abs(From.Y - To.Y);
}

float UAIActorsInteractions:s:CalculatePathProgress(const FIntPoint& Start, const FIntPoint& End, const FIntPoint& Current)
{
    float TotalDistance = CalculateHeuristic(Start, End);
    if (TotalDistance == 0) return 1.0f;

    float CurrentDistance = CalculateHeuristic(Start, Current);
    return FMath::Clamp(CurrentDistance / TotalDistance, 0.0f, 1.0f);
}

TArray<FIntPoint> UAIActorsInteractions::GetNeighbors(const FIntPoint& Node)
{
    return {
        FIntPoint(Node.X + 1, Node.Y),   // вправо
        FIntPoint(Node.X - 1, Node.Y),   // влево  
        FIntPoint(Node.X, Node.Y + 1),   // вперед
        FIntPoint(Node.X, Node.Y - 1)    // назад
    };
}

TArray<FVector> UAIActorsInteractions::ReconstructPath(const TMap<FIntPoint, FIntPoint>& CameFrom,
    const FIntPoint& Start, const FIntPoint& End, float CellSize, float StartZ, float TargetZ)
{
    TArray<FVector> Path;
    FIntPoint Current = End;

    while (Current != Start)
    {
        FVector WorldPosition = GridToWorld(Current, CellSize);

        // Интерполируем высоту между стартом и финишем
        float Alpha = CalculatePathProgress(Start, End, Current);
        WorldPosition.Z = FMath::Lerp(StartZ, TargetZ, Alpha);

        Path.Add(WorldPosition);

        const FIntPoint* Parent = CameFrom.Find(Current);
        if (Parent)
        {
            Current = *Parent;
        }
        else
        {
            break;
        }
    }

    // Добавляем стартовую точку с оригинальной высотой
    FVector StartWorldPos = GridToWorld(Start, CellSize);
    StartWorldPos.Z = StartZ;
    Path.Add(StartWorldPos);

    Algo::Reverse(Path);

    // Убеждаемся, что конечная точка имеет точную высоту цели
    if (Path.Num() > 0)
    {
        Path.Last().Z = TargetZ;
    }

    return Path;
}

FIntPoint UAIActorsInteractions::WorldToGrid(const FVector& WorldLocation, float CellSize)
{
    return FIntPoint(
        FMath::RoundToInt(WorldLocation.X / CellSize),
        FMath::RoundToInt(WorldLocation.Y / CellSize)
    );
}

FVector UAIActorsInteractions::GridToWorld(const FIntPoint& GridPosition, float CellSize)
{
    return FVector(
        GridPosition.X * CellSize,
        GridPosition.Y * CellSize,
        0.0f  // Базовая высота, будет переопределена в ReconstructPath
    );
}

bool UAIActorsInteractions::IsValidGridPosition(const FIntPoint& GridPosition)
{
    return true;
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
