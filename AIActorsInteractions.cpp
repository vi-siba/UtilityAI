#include "AIActorsInteractions.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

AActor* AIActorsInteractions::GetClosestActor(TSubclassOf<AActor> ActorClassToFind, AActor* ExecuterActor)
{
    if (!ActorClass || !ExecuterActor)
        return nullptr;

    //Reset of basic parametres
    AActor* ClosestActor = nullptr;
    float MinDistance = FLT_MAX;

    for (TActorIterator<AActor> It(GetWorld(), ActorClass); It; ++It)
    {
        AActor* Actor = *It;
        if (Actor != ExecuterActor)
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