#include "ZombieSpawner.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

AZombieSpawner::AZombieSpawner()
{

}

bool SpawnSingle(FVector PlacetoSpawn)
{
    if (!SpawnPoint)
        return false;

    SpawnObjectDisplacement.X = SpawnPoint->GetActorLocation().X + FMath::RandRange(-Displacement, Displacement);
    SpawnObjectDisplacement.Y = SpawnPoint->GetActorLocation().Y + FMath::RandRange(-Displacement, Displacement);
    SpawnObjectDisplacement.Z = SpawnPoint->GetActorLocation().Z;

    ACharacter* SpawnedActor = GetWorld()->SpawnActor<ACharacter>(ZombieToSpawnClass, SpawnLocation, SpawnRotation, SpawnParams);

    //AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(CablePlacementClass, SpawnObjectDisplacement, Rotation /* , SpawnParams */);

    if (SpawnedActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor spawned successfully!"));
        return true;
    }
    else
    {
        return false;
    }

}


AZombieSpawner::SpawnNPC()
{
    if (SpawnPoints.empty())
        return;

    /*
    for (int i = 0, i < sizeof(SpawnPoints) / sizeof(SpawnPoints[0]), i++)
    {
         
    }
    */

    for (int i = 0, i < ZombieBaseQuantity, i++)
    {
        while (!SpawnSingle(PlacetoSpawn))
        ///////////////////////////////////////////////
        // Zaglushka;
        ///////////////////////////////////////////////

    }

}

AZombieSpawner::AdditionalQuantity()
{
    if (UtilityCurves != nullptr)
    {
        for (const auto& [ParamName, Curve] : UtilityCurves)
        {
            if (!Curve) continue;

            const float* ParamValuePtr = Parameters.Find(ParamName);
            if (!ParamValuePtr) continue;

            float ParamValue = *ParamValuePtr;
            float UtilityFromCurve = Curve->GetFloatValue(ParamValue);
            float CurveModifier = CurveModifiersMap.FindRef(ParamName);
            float ModifiedUtility;

            //ModifiedUtility = UtilityFromCurve + CurveModifier + Distance;
            //TotalUtility += ModifiedUtility;
            //TotalUtility = Distance;

            DebugMessage.Append(FString::Printf(TEXT("\n - [%s]: %.2f → %.2f (%.2f)"), *ParamName.ToString(), ParamValue, UtilityFromCurve, CurveModifier));
        }

}
/*
float UAIBaseAction::CalculateUtility(const TMap<FName, float>& Parameters)
{
    float TotalUtility = 0.0f;

    FString DebugMessage = FString::Printf(TEXT("Action: %s"), *ActionName.ToString());

    // Calculation of TotalUtility, based on current NPC parameteres and action Curves
    if (UtilityCurves != nullptr)
    {
            for (const auto& [ParamName, Curve] : UtilityCurves)
    {
        if (!Curve) continue;

        const float* ParamValuePtr = Parameters.Find(ParamName);
        if (!ParamValuePtr) continue;

        float ParamValue = *ParamValuePtr;
        float UtilityFromCurve = Curve->GetFloatValue(ParamValue);
        float CurveModifier = CurveModifiersMap.FindRef(ParamName);

        ///!!! WIP
        ///!!! Test 
        float ModifiedUtility;
        if (ActorClass)
        {
        }
        else
        {
            ModifiedUtility = UtilityFromCurve + CurveModifier;
            TotalUtility += ModifiedUtility;

        }
            AActor* GetClosestActor = UAIActorsInteractions::GetClosestActor(ActorClass, ActionExecuterActor);
            AActor* ClosestActor = UAIActorsInteractions::GetClosestActor(ActorClass, ActionExecuterActor);

           // float Distance = FVector::Dist(GetClosestActor->GetActorLocation(), ActionExecuterActor->GetActorLocation());
            float Distance = (ActionExecuterActor->GetActorLocation() - GetClosestActor->GetActorLocation()).Size();
            ModifiedUtility = UtilityFromCurve + CurveModifier + Distance;
            TotalUtility += ModifiedUtility;
            TotalUtility = Distance;
        ///!!! WIP
        ///!!! Test 

        //TotalUtility += ModifiedUtility;

        DebugMessage.Append(FString::Printf(TEXT("\n - [%s]: %.2f → %.2f (%.2f)"),*ParamName.ToString(), ParamValue, UtilityFromCurve, CurveModifier));
    }
    }


    if (ActorClass)
    {
    AActor* GetClosestActor = UAIActorsInteractions::GetClosestActor(ActorClass, ActionExecuterActor);
    if (GetClosestActor)
    {
    float Distance = (ActionExecuterActor->GetActorLocation() - GetClosestActor->GetActorLocation()).Size();
    TotalUtility = Distance;

    }
    }

    DebugMessage.Append(FString::Printf(TEXT("\n = Total: %.2f"), TotalUtility));

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Cyan, DebugMessage);
    }

    return TotalUtility;
}

void UAIBaseAction::ApplyEffects(TMap<FName, float>& Parameters)
{
    for (const auto& [ParamName, EffectValue] : ParameterEffects)
    {
        float& ParamRef = Parameters.FindOrAdd(ParamName);
        ParamRef = ParamRef + EffectValue;
        //ParamRef = FMath::Clamp(ParamRef + EffectValue, 0.0f, 1.0f);
    }
}

TArray<FName> UAIBaseAction::GetRelevantParameters() const
{
    TArray<FName> Keys;
    UtilityCurves.GetKeys(Keys);
    return Keys;
}

void UAIBaseAction::AddOrUpdateCurveModifier(FName CurveName, float ModifierValue)
{
    CurveModifiersMap.Add(CurveName, ModifierValue); 
}

        */
