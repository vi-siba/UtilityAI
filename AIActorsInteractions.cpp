#include "AIActorsInteractions.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

AActor* UAIActorsInteractions::GetClosestActor(TSubclassOf<AActor> ActorClassToFind, AActor* ExecuterActor)
{
    if (!ActorClassToFind || !ExecuterActor)
        return nullptr;

    //Reset of basic parametres
    AActor* ClosestActor = nullptr;
    float MinDistance = FLT_MAX;

    for (TActorIterator<AActor> It(GetWorld(), ActorClassToFind); It; ++It)
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
