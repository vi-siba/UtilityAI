#include "AIBaseAction.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

UAIBaseAction::UAIBaseAction()
{
    ActionName = TEXT("BaseAction");
    
    ///////////////////////
    SetActionExecuterActor();

}

float UAIBaseAction::CalculateUtility(const TMap<FName, float>& Parameters)
{
    FString DebugMessage = FString::Printf(TEXT("Action: %s"), *ActionName.ToString());

    float TotalUtility = 0.0f;
    
    if (!ActionExecuterActor)
        return 0.0f;

    if (ActorClass)
    {
        AActor* GetClosestActor = UAIActorsInteractions::GetClosestActor(ActorClass, ActionExecuterActor);
        if (GetClosestActor == nullptr)
            return 0.0f;

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

/*
bool UAIBaseAction::SetActionExecuterActor(AActor* ExecuterActor)
{
    UObject* OuterObject = GetOuter();

    if (OuterObject)
    {
        AActor* OuterActor = Cast<AActor>(OuterObject);
        if (OuterActor)
        {
            ActionExecuterActor = OuterActor;
        }
    }

    if (!ExecuterActor)
        return false;

    ActionExecuterActor = ExecuterActor;
    return true;
}
*/

// При инициализации - первый раз не устанавливается, так как сначала делают копии UObject'ов, а потом только Pawn
// Сделать проверку/делей/инициализацию при спавне пешки в контроллере, а не при инициализации действия
bool UAIBaseAction::SetActionExecuterActor()
{
    UObject* OuterObject = GetOuter();

    if (!OuterObject)
        return false;

    AActor* OuterActor = Cast<AActor>(OuterObject);
    if (OuterActor)
    {
        ActionExecuterActor = OuterActor;

        AController* OuterController = Cast<AController>(OuterObject);
        if (OuterController)
        {
            PawnActor = OuterController->GetPawn(); // Правильно!
        }

        return true;
    }
    else
        return false;
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


