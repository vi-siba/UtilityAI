#include "AIActorsInteractions.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Engine/World.h"
#include "NavMesh/RecastNavMesh.h"

#include "AI/Navigation/NavQueryFilter.h"


struct FPathNode3D
{
    FIntPoint GridPosition;
    float Height;

    FPathNode3D(const FIntPoint& InGridPosition, float InHeight)
        : GridPosition(InGridPosition), Height(InHeight) {
    }

    bool operator==(const FPathNode3D& Other) const
    {
        return GridPosition == Other.GridPosition && FMath::IsNearlyEqual(Height, Other.Height, 1.0f);
    }

    friend uint32 GetTypeHash(const FPathNode3D& Node)
    {
        return HashCombine(GetTypeHash(Node.GridPosition), GetTypeHash(Node.Height));
    }
};

TArray<FVector> UAIActorsInteractions::FindPath(
    UObject* WorldContextObject,
    const FVector& StartLocation,
    const FVector& TargetLocation,
    float CellSize,
    float HeightAboveGround,
    AActor* CallingActor)
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

    // Получаем высоту поверхности для стартовой и конечной точек, игнорируя вызывающий актор для стартовой точки
    float StartSurfaceHeight = GetSurfaceHeight(WorldContextObject, StartLocation, CellSize, CallingActor); // Игнорируем вызывающий актор
    float TargetSurfaceHeight = GetSurfaceHeight(WorldContextObject, TargetLocation, CellSize, nullptr); // Для конечной точки не игнорируем

    FIntPoint StartGrid = WorldToGrid(StartLocation, CellSize);
    FIntPoint TargetGrid = WorldToGrid(TargetLocation, CellSize);

    float StartHeight = StartSurfaceHeight + HeightAboveGround;
    float TargetHeight = TargetSurfaceHeight + HeightAboveGround;

    UE_LOG(LogTemp, Log, TEXT("FindPath: StartGrid (%d, %d) at Z=%.2f, TargetGrid (%d, %d) at Z=%.2f, HeightAboveGround: %.2f"),
        StartGrid.X, StartGrid.Y, StartHeight,
        TargetGrid.X, TargetGrid.Y, TargetHeight,
        HeightAboveGround);

    if (StartGrid == TargetGrid && FMath::IsNearlyEqual(StartHeight, TargetHeight, 10.0f))
    {
        UE_LOG(LogTemp, Log, TEXT("Start and target are the same position"));
        FVector FinalPosition = TargetLocation;
        FinalPosition.Z = TargetHeight;
        Path.Add(FinalPosition);
        return Path;
    }

    // Создаем 3D узлы для старта и цели
    FPathNode3D StartNode(StartGrid, StartHeight);
    FPathNode3D TargetNode(TargetGrid, TargetHeight);

    // Проверяем доступность стартовой и конечной точек
    bool bStartWalkable = IsPositionWalkableAtSpecificHeight(WorldContextObject, StartGrid, StartHeight, CellSize, CallingActor);
    bool bTargetWalkable = IsPositionWalkableAtSpecificHeight(WorldContextObject, TargetGrid, TargetHeight, CellSize, nullptr);

    UE_LOG(LogTemp, Log, TEXT("Start walkable: %s, Target walkable: %s"),
        bStartWalkable ? TEXT("True") : TEXT("False"),
        bTargetWalkable ? TEXT("True") : TEXT("False"));

    if (!bStartWalkable || !bTargetWalkable)
    {
        UE_LOG(LogTemp, Error, TEXT("Start or target position is not walkable"));
        return Path;
    }

    // Структуры данных для 3D A*
    TMap<FPathNode3D, float> GCostMap;
    TArray<FPathNode3D> OpenSet;
    TMap<FPathNode3D, FPathNode3D> CameFrom;

    GCostMap.Add(StartNode, 0.0f);
    OpenSet.Add(StartNode);

    int32 IterationCount = 0;
    const int32 MaxIterations = 20000;

    while (OpenSet.Num() > 0 && IterationCount < MaxIterations)
    {
        IterationCount++;

        // Находим узел с минимальной F стоимостью
        int32 CurrentIndex = 0;
        FPathNode3D CurrentNode = OpenSet[0];
        float CurrentFCost = GCostMap.FindRef(CurrentNode) + CalculateHeuristic3D(CurrentNode, TargetNode);

        for (int32 i = 1; i < OpenSet.Num(); i++)
        {
            FPathNode3D TestNode = OpenSet[i];
            float TestFCost = GCostMap.FindRef(TestNode) + CalculateHeuristic3D(TestNode, TargetNode);

            if (TestFCost < CurrentFCost)
            {
                CurrentNode = TestNode;
                CurrentIndex = i;
                CurrentFCost = TestFCost;
            }
        }

        // Визуализация текущего узла (для отладки)
        if (World && IterationCount % 100 == 0)
        {
            FVector DebugPos = GridToWorld(CurrentNode.GridPosition, CellSize);
            DebugPos.Z = CurrentNode.Height;
            DrawDebugPoint(World, DebugPos, 15.0f, FColor::Yellow, false, 2.0f);
        }

        if (CurrentNode == TargetNode)
        {
            UE_LOG(LogTemp, Log, TEXT("3D Path found! Iterations: %d, Path length: %d nodes"),
                IterationCount, CameFrom.Num() + 1);
            return ReconstructPath3D(CameFrom, StartNode, TargetNode, CellSize);
        }

        OpenSet.RemoveAt(CurrentIndex);

        // Получаем 3D соседей с учетом высоты
        TArray<FPathNode3D> Neighbors = GetNeighbors3D(CurrentNode, CellSize, HeightAboveGround, WorldContextObject, CallingActor);

        for (const FPathNode3D& Neighbor : Neighbors)
        {
            if (!IsPositionWalkableAtSpecificHeight(WorldContextObject, Neighbor.GridPosition, Neighbor.Height, CellSize, CallingActor))
            {
                continue;
            }

            float TentativeGCost = GCostMap.FindRef(CurrentNode) + CalculateDistance3D(CurrentNode, Neighbor, CellSize);
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
        UE_LOG(LogTemp, Warning, TEXT("3D Path finding exceeded maximum iterations (%d)"), MaxIterations);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("3D Path not found after %d iterations"), IterationCount);
    }

    return Path;
}


float UAIActorsInteractions::GetSurfaceHeight(UObject* WorldContextObject, const FVector& WorldLocation, float CellSize, AActor* ActorToIgnore)
{
    if (!WorldContextObject) return WorldLocation.Z;

    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World) return WorldLocation.Z;

    float TraceDistance = 1000.0f;
    FVector TraceStart = WorldLocation + FVector(0, 0, TraceDistance);
    FVector TraceEnd = WorldLocation - FVector(0, 0, TraceDistance);

    TArray<AActor*> ActorsToIgnore;

    // Добавляем актор для игнорирования, если он передан
    if (ActorToIgnore)
    {
        ActorsToIgnore.Add(ActorToIgnore);
    }

    FHitResult HitResult;

    bool bHit = UKismetSystemLibrary::LineTraceSingle(
        World,
        TraceStart,
        TraceEnd,
        UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
        false,
        ActorsToIgnore,
        EDrawDebugTrace::None,
        HitResult,
        true
    );

    if (bHit)
    {
        return HitResult.ImpactPoint.Z;
    }

    return WorldLocation.Z;
}



// Получение 3D соседей с учетом высоты поверхности
TArray<UAIActorsInteractions::FPathNode3D> UAIActorsInteractions::GetNeighbors3D(
    const FPathNode3D& Node,
    float CellSize,
    float HeightAboveGround,
    UObject* WorldContextObject,
    AActor* ActorToIgnore)  // Добавляем параметр для игнорирования актора
{
    TArray<FPathNode3D> Neighbors;

    TArray<FIntPoint> GridNeighbors = GetNeighbors(Node.GridPosition);

    for (const FIntPoint& GridNeighbor : GridNeighbors)
    {
        // Получаем высоту поверхности для соседней точки, игнорируя актор если нужно
        FVector WorldPos = GridToWorld(GridNeighbor, CellSize);
        float SurfaceHeight = GetSurfaceHeight(WorldContextObject, WorldPos, CellSize, ActorToIgnore);  // Передаем ActorToIgnore
        float NeighborHeight = SurfaceHeight + HeightAboveGround;

        // Также проверяем точки с небольшим отклонением по высоте для лучшего обхода препятствий
        TArray<float> HeightVariations = { 0.0f, 25.0f, -25.0f, 50.0f, -50.0f };

        for (float HeightVariation : HeightVariations)
        {
            float TestHeight = NeighborHeight + HeightVariation;

            // Проверяем, чтобы высота не была слишком низкой
            if (TestHeight < SurfaceHeight + 10.0f) continue;

            FPathNode3D NeighborNode(GridNeighbor, TestHeight);

            // Проверяем проходимость на этой высоте, игнорируя актор если нужно
            if (IsPositionWalkableAtSpecificHeight(WorldContextObject, GridNeighbor, TestHeight, CellSize, ActorToIgnore))  // Передаем ActorToIgnore
            {
                Neighbors.Add(NeighborNode);
            }
        }
    }

    return Neighbors;
}

// Расчет расстояния между 3D узлами
float UAIActorsInteractions::CalculateDistance3D(const FPathNode3D& From, const FPathNode3D& To, float CellSize)
{
    // Горизонтальное расстояние
    float HorizontalDistance = CalculateDistance(From.GridPosition, To.GridPosition, CellSize);

    // Вертикальное расстояние (с меньшим весом)
    float VerticalDistance = FMath::Abs(From.Height - To.Height);

    // Комбинируем расстояния (можно настроить веса)
    return HorizontalDistance + VerticalDistance * 0.5f;
}

// Эвристика для 3D
float UAIActorsInteractions::CalculateHeuristic3D(const FPathNode3D& From, const FPathNode3D& To)
{
    // Горизонтальное расстояние
    int32 DX = FMath::Abs(From.GridPosition.X - To.GridPosition.X);
    int32 DY = FMath::Abs(From.GridPosition.Y - To.GridPosition.Y);
    float F_DX = static_cast<float>(DX);
    float F_DY = static_cast<float>(DY);
    float HorizontalDist = FMath::Sqrt(F_DX * F_DX + F_DY * F_DY);

    // Вертикальное расстояние
    float VerticalDist = FMath::Abs(From.Height - To.Height);

    // Комбинируем (можно настроить веса)
    return HorizontalDist + VerticalDist * 0.3f;
}

// Проверка проходимости на конкретной высоте
bool UAIActorsInteractions::IsPositionWalkableAtSpecificHeight(UObject* WorldContextObject, const FIntPoint& Position, float Height, float CellSize, AActor* ActorToIgnore)
{
    if (!WorldContextObject) return true;

    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World) return true;

    FVector WorldPosition = GridToWorld(Position, CellSize);
    WorldPosition.Z = Height;

    // Настраиваем параметры трассировки
    float TraceRadius = FMath::Max(CellSize * 0.3f, 10.0f);
    float VerticalClearance = FMath::Max(CellSize * 0.3f, 30.0f);

    FVector TraceStart = WorldPosition + FVector(0, 0, VerticalClearance * 0.5f);
    FVector TraceEnd = WorldPosition - FVector(0, 0, VerticalClearance * 0.5f);

    TArray<AActor*> ActorsToIgnore;

    // Добавляем актор для игнорирования, если он передан
    if (ActorToIgnore)
    {
        ActorsToIgnore.Add(ActorToIgnore);
    }

    TArray<FHitResult> HitResults;

    // Трассируем только по статическим объектам
    bool bHasObstacle = UKismetSystemLibrary::SphereTraceMulti(
        World,
        TraceStart,
        TraceEnd,
        TraceRadius,
        UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
        false,
        ActorsToIgnore,
        EDrawDebugTrace::None,
        HitResults,
        true
    );

    // Анализируем результаты трассировки
    if (bHasObstacle)
    {
        for (const FHitResult& Hit : HitResults)
        {
            UPrimitiveComponent* HitComponent = Hit.GetComponent();
            if (HitComponent && (HitComponent->IsA<UStaticMeshComponent>() || HitComponent->Mobility == EComponentMobility::Static))
            {
                // Проверяем, не является ли это полом/землей
                bool bIsFloor = (Hit.ImpactNormal.Z > 0.7f);

                if (!bIsFloor)
                {
                    // Это препятствие (стена, объект и т.д.)
                    return false;
                }
            }
        }

        return true;
    }

    return true;
}
// Реконструкция 3D пути
TArray<FVector> UAIActorsInteractions::ReconstructPath3D(
    const TMap<FPathNode3D, FPathNode3D>& CameFrom,
    const FPathNode3D& Start,
    const FPathNode3D& End,
    float CellSize)
{
    TArray<FVector> Path;
    FPathNode3D Current = End;

    UE_LOG(LogTemp, Log, TEXT("Reconstructing 3D path"));

    // Собираем все точки пути в правильном порядке (от старта к финишу)
    TArray<FPathNode3D> PathPoints;

    // Сначала идем от конца к началу
    while (!(Current == Start))
    {
        PathPoints.Insert(Current, 0);
        const FPathNode3D* Parent = CameFrom.Find(Current);
        if (Parent)
        {
            Current = *Parent;
        }
        else
        {
            break;
        }
    }
    PathPoints.Insert(Start, 0);

    // Преобразуем 3D узлы в мировые координаты
    for (const FPathNode3D& Point : PathPoints)
    {
        FVector WorldPosition = GridToWorld(Point.GridPosition, CellSize);
        WorldPosition.Z = Point.Height;
        Path.Add(WorldPosition);

        UE_LOG(LogTemp, VeryVerbose, TEXT("3D Path point (%d, %d, %.2f)"),
            Point.GridPosition.X, Point.GridPosition.Y, Point.Height);
    }

    UE_LOG(LogTemp, Log, TEXT("Reconstructed 3D path with %d points"), Path.Num());

    // Логируем все точки пути
    for (int32 i = 0; i < Path.Num(); i++)
    {
        UE_LOG(LogTemp, Log, TEXT("3D Path point %d: (%.2f, %.2f, %.2f)"),
            i, Path[i].X, Path[i].Y, Path[i].Z);
    }

    return Path;
}


// Остальные функции без изменений
float UAIActorsInteractions::CalculateDistance(const FIntPoint& From, const FIntPoint& To, float CellSize)
{
    int32 DX = FMath::Abs(From.X - To.X);
    int32 DY = FMath::Abs(From.Y - To.Y);

    if (DX > 0 && DY > 0)
    {
        return FMath::Sqrt(2.0f) * CellSize;
    }
    else
    {
        return CellSize;
    }
}

float UAIActorsInteractions::CalculateHeuristic(const FIntPoint& From, const FIntPoint& To)
{
    int32 DX = FMath::Abs(From.X - To.X);
    int32 DY = FMath::Abs(From.Y - To.Y);
    float F_DX = static_cast<float>(DX);
    float F_DY = static_cast<float>(DY);
    return FMath::Sqrt(F_DX * F_DX + F_DY * F_DY);
}

TArray<FIntPoint> UAIActorsInteractions::GetNeighbors(const FIntPoint& Node)
{
    return {
        FIntPoint(Node.X + 1, Node.Y + 1),
        FIntPoint(Node.X - 1, Node.Y - 1),
        FIntPoint(Node.X + 1, Node.Y - 1),
        FIntPoint(Node.X - 1, Node.Y + 1),
        FIntPoint(Node.X + 1, Node.Y),
        FIntPoint(Node.X - 1, Node.Y),
        FIntPoint(Node.X, Node.Y + 1),
        FIntPoint(Node.X, Node.Y - 1)
    };
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
        0.0f
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
