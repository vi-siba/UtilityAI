#include "AIActorsInteractions.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

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