#include "AIModifierComponent.h"
#include "AIBaseAction.h"

UAIModifierComponent::UAIModifierComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAIModifierComponent::AddCurveModifier(FName ActionName, FName CurveName, float ModifierValue)
{
    // Обращаемся к вложенному TMap через поле CurveModifiers
    CurveModifiers.FindOrAdd(ActionName).CurveModifiers.Add(CurveName, ModifierValue);
}

void UAIModifierComponent::RemoveCurveModifier(FName ActionName, FName CurveName)
{
    if (FCurveModifierMap* ModifiersStruct = CurveModifiers.Find(ActionName))
    {
        TMap<FName, float>& Modifiers = ModifiersStruct->CurveModifiers; // обращаемся к вложенному мапу

        Modifiers.Remove(CurveName);

        if (Modifiers.Num() == 0)
        {
            CurveModifiers.Remove(ActionName);
        }
    }
}

void UAIModifierComponent::ApplyModifiers(const TArray<UAIBaseAction*>& Actions)
{
    for (UAIBaseAction* Action : Actions)
    {
        if (!Action) continue;

        FName ActionName = Action->GetActionName();

        if (const FCurveModifierMap* ModifiersStruct = CurveModifiers.Find(ActionName))
        {
            const TMap<FName, float>& Modifiers = ModifiersStruct->CurveModifiers;

            // Для каждой кривой в действии проверяем есть ли модификатор, и применяем
            for (auto& CurvePair : Action->UtilityCurves)
            {
                const float* ModifierPtr = Modifiers.Find(CurvePair.Key);
                if (ModifierPtr)
                {
                    // Добавим поле в UAIBaseAction для хранения модификаторов по кривым
                    Action->AddOrUpdateCurveModifier(CurvePair.Key, *ModifierPtr);
                }
            }
        }
    }
}

void UAIModifierComponent::ClearAllModifiers()
{
    CurveModifiers.Empty();
}


void UAIModifierComponent::ModifyCurveUtility(FName ActionName, FName CurveName, float DeltaModifier)
{
    FCurveModifierMap& ModifierMap = CurveModifiers.FindOrAdd(ActionName);
    float& CurrentModifier = ModifierMap.CurveModifiers.FindOrAdd(CurveName);
    CurrentModifier += DeltaModifier;

    // Если результат стал 0, удалим модификатор (чистим мусор)
    if (FMath::IsNearlyZero(CurrentModifier, KINDA_SMALL_NUMBER))
    {
        ModifierMap.CurveModifiers.Remove(CurveName);
    }

    // Если всё почищено — удаляем саму структуру
    if (ModifierMap.CurveModifiers.Num() == 0)
    {
        CurveModifiers.Remove(ActionName);
    }
}
