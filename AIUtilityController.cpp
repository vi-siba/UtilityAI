#include "AIUtilityController.h"
#include "TimerManager.h"

AAIUtilityController::AAIUtilityController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ModifierComponent = CreateDefaultSubobject<UAIModifierComponent>(TEXT("ModifierComponent"));
    PrimaryActorTick.bCanEverTick = true;
}

void AAIUtilityController::BeginPlay()
{
    Super::BeginPlay();

    InitializeActions();
    GetWorldTimerManager().SetTimer(UtilityEvaluationTimer, this, &AAIUtilityController::EvaluateAndAct, 2.0f, true);
}

void AAIUtilityController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAIUtilityController::InitializeActions()
{
    // Реализация инициализации действий
}

void AAIUtilityController::EvaluateAndAct()
{
    if (!ModifierComponent) return;
    TArray<UAIBaseAction*> Actions;
    for (UAIBaseAction* Action : Actions)
    {
        if (!Action) continue;

        // Узнаём, какие параметры нужны действию
        for (const FName& ParamName : Action->GetRelevantParameters())
        {
            if (!CurrentParameters.Contains(ParamName))
            {
                CurrentParameters.Add(ParamName, 0.5f); // Стартовое значение по умолчанию
            }
        }
    }
    // Создание всех возможных действий
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

    // Применение модификаторов
    ModifierComponent->ApplyModifiers(Actions);

    // Оценка полезности и выбор лучшего действия
    UAIBaseAction* BestAction = nullptr;
    float HighestUtility = -FLT_MAX;

    for (UAIBaseAction* Action : Actions)
    {
        if (Action)
        {
            float Utility = Action->CalculateUtility(CurrentParameters); // <--- твоя карта параметров
            if (Utility > HighestUtility)
            {
                HighestUtility = Utility;
                BestAction = Action;
            }
        }
    }

    // Выполнение лучшего действия
    if (BestAction)
    {
        BestAction->Execute();
        BestAction->ApplyEffects(CurrentParameters); // <--- не забываем обновить параметры
    }
}
