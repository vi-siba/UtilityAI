#include "AIBaseAction.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

UAIBaseAction::UAIBaseAction()
{
    ActionName = TEXT("BaseAction");
 //   AIActorsInteractions = NewObject<UAIActorsInteractions>(this);

}

float UAIBaseAction::CalculateUtility(const TMap<FName, float>& Parameters)
{
    float TotalUtility = 0.0f;

    FString DebugMessage = FString::Printf(TEXT("Action: %s"), *ActionName.ToString());

    // Calculation of TotalUtility, based on current NPC parameteres and action Curves
    for (const auto& [ParamName, Curve] : UtilityCurves)
    {
        if (!Curve) continue;

        const float* ParamValuePtr = Parameters.Find(ParamName);
        if (!ParamValuePtr) continue;

        float ParamValue = *ParamValuePtr;
        float UtilityFromCurve = Curve->GetFloatValue(ParamValue);
        float CurveModifier = CurveModifiersMap.FindRef(ParamName);

        float ModifiedUtility = UtilityFromCurve + CurveModifier;
        TotalUtility += ModifiedUtility;

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

void UAIBaseAction::Execute_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("Executing action: %s"), *ActionName.ToString());
}

void UAIBaseAction::SetActionExecuterActor(AActor* ExecuterActor)
{
    ActionExecuterActor = ExecuterActor;
}
