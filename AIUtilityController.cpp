#include "AIUtilityController.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "AIModifierComponent.h"

AAIUtilityController::AAIUtilityController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
  //  ModifierComponent = CreateDefaultSubobject<UAIModifierComponent>(TEXT("ModifierComponent"));
    PrimaryActorTick.bCanEverTick = true;
}

void AAIUtilityController::BeginPlay()
{
    Super::BeginPlay();

    //InitializeActions();
    GetWorldTimerManager().SetTimer(UtilityEvaluationTimer, this, &AAIUtilityController::EvaluateAndAct, 2.0f, true);
}

void AAIUtilityController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}


void AAIUtilityController::EvaluateAndAct()
{
   // if (!ModifierComponent) return;


    TArray<UAIBaseAction*> Actions;
    for (UAIBaseAction* Action : Actions)
    {
        if (!Action) continue;
        for (const FName& ParamName : Action->GetRelevantParameters())
        {
            if (!CurrentParameters.Contains(ParamName))
            {
                CurrentParameters.Add(ParamName, 0.5f); // Стартовое значение по умолчанию
            }
        }
    }
    for (TSubclassOf<UAIBaseAction> ActionClass : ActionClasses)
    {
        if (ActionClass)
        {
            UAIBaseAction* NewAction = NewObject<UAIBaseAction>(this, ActionClass);
            if (NewAction)
            {
                Actions.Add(NewAction);
            }
        }
    }

    if (ModifierComponent)
    {
        ModifierComponent->ApplyModifiers(Actions);
    }

    UAIBaseAction* BestAction = nullptr;
    float HighestUtility = -FLT_MAX;

    for (UAIBaseAction* Action : Actions)
    {
        if (Action)
        {
            float Utility = Action->CalculateUtility(CurrentParameters);
            if (Utility > HighestUtility)
            {
                HighestUtility = Utility;
                BestAction = Action;
            }
        }
    }

    if (BestAction)
    {
        BestAction->Execute();
        BestAction->ApplyEffects(CurrentParameters);
    }
}

void AAIUtilityController::SetModifierComponent(UAIModifierComponent* ModifierComp)
{
    ModifierComponent = ModifierComp;
}

void AAIUtilityController::InitializeActions()
{
}
