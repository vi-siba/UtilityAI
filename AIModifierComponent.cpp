#include "AIModifierComponent.h"
#include "AIBaseAction.h"

UAIModifierComponent::UAIModifierComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAIModifierComponent::AddActionModifier(FName ActionName, float ModifierValue)
{
    ActionModifiers.Add(ActionName, ModifierValue);
}

void UAIModifierComponent::RemoveActionModifier(FName ActionName)
{
    ActionModifiers.Remove(ActionName);
}

void UAIModifierComponent::ApplyModifiers(const TArray<UAIBaseAction*>& Actions)
{
    for (UAIBaseAction* Action : Actions)
    {
        if (Action && ActionModifiers.Contains(Action->GetActionName()))
        {
            Action->SetUtilityModifier(ActionModifiers[Action->GetActionName()]);
        }
    }
}

void UAIModifierComponent::ClearAllModifiers()
{
    ActionModifiers.Empty();
}
