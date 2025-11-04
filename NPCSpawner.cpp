#include "ZombieSpawner.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

AZombieSpawner::AZombieSpawner()
{

}

void AZombieSpawner::SpawnNPC()
{
    if (SpawnPoints.IsEmpty())
        return;

    if (!ZombieClass)
        return;

    int AdditionalNPC = AdditionalQuantity();

    NPCQuantity += AdditionalNPC;

    for (int i = 0; i < SpawnPoints.Num(); i++)
    {
        for (int j = 0; j < NPCQuantity; j++)
        {
            int attempts = 0;
            const int maxAttempts = 5;

            while (!SpawnSingle(SpawnPoints[i]) && attempts < maxAttempts)
            {
                attempts++;
            }
        }
    }

}

bool AZombieSpawner::SpawnSingle(AActor* SpawnPointActor)
{
    if (!SpawnPointActor)
        return false;

    FVector SpawnNPCLocation;

    SpawnNPCLocation.X = SpawnPointActor->GetActorLocation().X + FMath::RandRange(-Displacement, Displacement);
    SpawnNPCLocation.Y = SpawnPointActor->GetActorLocation().Y + FMath::RandRange(-Displacement, Displacement);
    SpawnNPCLocation.Z = SpawnPointActor->GetActorLocation().Z;

    FRotator SpawnNPCRotation = SpawnPointActor->GetActorRotation();

    if (!ZombieClass)
        return false;

    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ZombieClass, SpawnNPCLocation, SpawnNPCRotation /* SpawnParams*/);

    if (SpawnedActor)
    {
      //  UE_LOG(LogTemp, Warning, TEXT("Actor spawned successfully"));
        return true;
    }
    else
    {
        //  UE_LOG(LogTemp, Warning, TEXT("Actor wasn't spawned"));
        return false;
    }

}

int AZombieSpawner::AdditionalQuantity()
{
   int AdditionalNPCQuantity=0;

   if (UtilityCurves.IsEmpty())
       return 0;

   if (CurrentParameters.IsEmpty())
       return 0;

   for (const auto& [ParamName, Curve] : UtilityCurves)
        {
            if (!Curve) continue;

            const float* ParamValuePtr = CurrentParameters.Find(ParamName);
            if (!ParamValuePtr) continue;
            
            float ParamValue = *ParamValuePtr;
            float UtilityFromCurve = Curve->GetFloatValue(ParamValue);

            AdditionalNPCQuantity += abs(UtilityFromCurve);
        }
   return AdditionalNPCQuantity;
}