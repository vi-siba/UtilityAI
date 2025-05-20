#include "AIModifierComponent.h"
#include "AIBaseAction.h"

UAIModifierComponent::UAIModifierComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAIModifierComponent::AddCurveModifier(FName ActionName, FName CurveName, float ModifierValue)
{
    // ���������� � ���������� TMap ����� ���� CurveModifiers
    CurveModifiers.FindOrAdd(ActionName).CurveModifiers.Add(CurveName, ModifierValue);
}

void UAIModifierComponent::RemoveCurveModifier(FName ActionName, FName CurveName)
{
    if (FCurveModifierMap* ModifiersStruct = CurveModifiers.Find(ActionName))
    {
        TMap<FName, float>& Modifiers = ModifiersStruct->CurveModifiers; // ���������� � ���������� ����

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

            // ��� ������ ������ � �������� ��������� ���� �� �����������, � ���������
            for (auto& CurvePair : Action->UtilityCurves)
            {
                const float* ModifierPtr = Modifiers.Find(CurvePair.Key);
                if (ModifierPtr)
                {
                    // ������� ���� � UAIBaseAction ��� �������� ������������� �� ������
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

    // ���� ��������� ���� 0, ������ ����������� (������ �����)
    if (FMath::IsNearlyZero(CurrentModifier, KINDA_SMALL_NUMBER))
    {
        ModifierMap.CurveModifiers.Remove(CurveName);
    }

    // ���� �� �������� � ������� ���� ���������
    if (ModifierMap.CurveModifiers.Num() == 0)
    {
        CurveModifiers.Remove(ActionName);
    }
}
