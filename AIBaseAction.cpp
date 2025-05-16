#include "AIBaseAction.h"
#include "Engine/Engine.h"

UAIBaseAction::UAIBaseAction()
{
    ActionName = TEXT("BaseAction");
}

float UAIBaseAction::CalculateUtility(const TMap<FName, float>& Parameters)
{
    float TotalUtility = 0.0f;

    FString DebugMessage = FString::Printf(TEXT("Action: %s"), *ActionName.ToString());

    for (const auto& CurvePair : UtilityCurves)
    {
        const FName& ParamName = CurvePair.Key;
        UCurveFloat* Curve = CurvePair.Value;

        if (Curve && Parameters.Contains(ParamName))
        {
            float ParamValue = Parameters[ParamName];
            float UtilityFromCurve = Curve->GetFloatValue(ParamValue);
            TotalUtility += UtilityFromCurve;

            // ?? Выводим вклад этой кривой
            FString CurveContribution = FString::Printf(TEXT("\n - [%s]: %.2f ? %.2f"),
                *ParamName.ToString(), ParamValue, UtilityFromCurve);

            DebugMessage.Append(CurveContribution);
        }
    }

    // Учитываем модификатор
    TotalUtility += UtilityModifier;
    DebugMessage.Append(FString::Printf(TEXT("\n + Modifier: %.2f"), UtilityModifier));

    // ?? Общая полезность
    DebugMessage.Append(FString::Printf(TEXT("\n = Total: %.2f"), TotalUtility));

    // ?? Выводим всё на экран
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Cyan, DebugMessage);
    }

    return TotalUtility;
}


void UAIBaseAction::ApplyEffects(TMap<FName, float>& Parameters)
{
    for (const auto& EffectPair : ParameterEffects)
    {
        if (Parameters.Contains(EffectPair.Key))
        {
            Parameters[EffectPair.Key] = FMath::Clamp(Parameters[EffectPair.Key] + EffectPair.Value, 0.0f, 1.0f);
        }
    }
}

void UAIBaseAction::Execute_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("Executing action: %s"), *ActionName.ToString());
}

TArray<FName> UAIBaseAction::GetRelevantParameters() const
{
    TArray<FName> Keys;
    UtilityCurves.GetKeys(Keys);
    return Keys;
}
