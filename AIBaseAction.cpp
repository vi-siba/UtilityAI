#include "AIBaseAction.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

UAIBaseAction::UAIBaseAction()
{
    ActionName = TEXT("BaseAction");

}

float UAIBaseAction::CalculateUtility(const TMap<FName, float>& Parameters)
{
    FString DebugMessage = FString::Printf(TEXT("Action: %s"), *ActionName.ToString());

    float TotalUtility = 0.0f;
    
    if (ActorClass)
    {
        AActor* GetClosestActor = UAIActorsInteractions::GetClosestActor(ActorClass, ActionExecuterActor);
        float Distance = (ActionExecuterActor->GetActorLocation() - GetClosestActor->GetActorLocation()).Size();
        TotalUtility += Distance;
    }


    if (UtilityCurves.IsEmpty())
        return TotalUtility;

    // Calculation of TotalUtility, based on current NPC parameteres and action Curves
    for (const auto& [ParamName, Curve] : UtilityCurves)
    {
        if (!Curve) continue;

        const float* ParamValuePtr = Parameters.Find(ParamName);
        if (!ParamValuePtr) continue;

        float ParamValue = *ParamValuePtr;
        float UtilityFromCurve = Curve->GetFloatValue(ParamValue);
        float CurveModifier = CurveModifiersMap.FindRef(ParamName);
        
        TotalUtility += UtilityFromCurve + CurveModifier;

        DebugMessage.Append(FString::Printf(TEXT("\n - [%s]: %.2f → %.2f (%.2f)"),*ParamName.ToString(), ParamValue, UtilityFromCurve, CurveModifier));
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

void UAIBaseAction::Execute_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("Executing action: %s"), *ActionName.ToString());
}

void UAIBaseAction::SetActionExecuterActor(AActor* ExecuterActor)
{
    ActionExecuterActor = ExecuterActor;
}

/*
bool UAIBaseAction::ObstaclesAlongWay(FVector Destination)
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

