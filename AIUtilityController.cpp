﻿#include "AIUtilityController.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "AIModifierComponent.h"

AAIUtilityController::AAIUtilityController(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
  //  ModifierComponent = CreateDefaultSubobject<UAIModifierComponent>(TEXT("ModifierComponent"));
    PrimaryActorTick.bCanEverTick = true;
}

void AAIUtilityController::BeginPlay()
{
    Super::BeginPlay();
    GetWorldTimerManager().SetTimer(UtilityEvaluationTimer, this, &AAIUtilityController::EvaluateAndAct, 2.0f, true);
}

void AAIUtilityController::EvaluateAndAct()
{
    TArray<UAIBaseAction*> Actions;
    CreateAndInitializeActions(Actions);

    if (ModifierComponent)
    {
        ModifierComponent->ApplyModifiers(Actions);
    }

    UAIBaseAction* BestAction = nullptr;
    float HighestUtility = -FLT_MAX;

    for (UAIBaseAction* Action : Actions)
    {
        const float Utility = Action->CalculateUtility(CurrentParameters);
        if (Utility > HighestUtility)
        {
            HighestUtility = Utility;
            BestAction = Action;
        }
    }

    if (BestAction)
    {
        BestAction->Execute();
        BestAction->ApplyEffects(CurrentParameters);
    }
}


void AAIUtilityController::CreateAndInitializeActions(TArray<UAIBaseAction*>& OutActions)
{
    for (TSubclassOf<UAIBaseAction> ActionClass : ActionClasses)
    {
        if (!ActionClass) continue;

        UAIBaseAction* NewAction = NewObject<UAIBaseAction>(this, ActionClass);
        if (!NewAction) continue;

        for (const FName& ParamName : NewAction->GetRelevantParameters())
        {
            CurrentParameters.FindOrAdd(ParamName, 0.5f);
        }

        OutActions.Add(NewAction);
    }
}


void AAIUtilityController::SetModifierComponent(UAIModifierComponent* ModifierComp)
{
    ModifierComponent = ModifierComp;
}

