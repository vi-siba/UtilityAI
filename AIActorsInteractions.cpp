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

bool UAIActorsInteractions::ObstaclesAlongWay(AActor* ActionExecuterActor, FVector Destination, float MaxDistanceToDestination)
{
    if (!ActionExecuterActor)
    {
        return true;
    }

    TArray<FVector> PathPoints;

    UWorld* World = ActionExecuterActor->GetWorld();
    
    if (!World)
        return true;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
    if (!NavSys)
        return true;

    UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(World, ActionExecuterActor->GetActorLocation(), Destination);
    if (NavPath && NavPath->IsValid())
    {
        PathPoints = NavPath->PathPoints;
    }

    int numberOfPoints = sizeof(PathPoints) / sizeof(PathPoints[0]);

    float Distance = FVector::Dist(PathPoints[numberOfPoints - 1], Destination);
    if (Distance < MaxDistanceToDestination)
        return false;
    else
        return true;
}
