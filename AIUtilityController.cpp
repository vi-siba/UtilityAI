#include "AIUtilityController.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "AIModifierComponent.h"

#include "EngineUtils.h"


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
        BestAction->SetActionExecuterActor(GetPawn());

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

float AAIUtilityController::FindClosestActorOfClass(TSubclassOf<AActor> ActorClass)
{
        AActor* ClosestActor = nullptr;
        float MinDistance = FLT_MAX;

        for (TActorIterator<AActor> It(GetWorld(), ActorClass); It; ++It)
        {
            AActor* Actor = *It;
            if (Actor != this)
            {
                float Distance = FVector::Dist(Actor->GetActorLocation(), GetPawn()->GetActorLocation());
                if (Distance < MinDistance)
                {
                    MinDistance = Distance;
                    ClosestActor = Actor;
                }
            }
    }
        return MinDistance;
}

void AAIUtilityController::SetParamNewValue(const FName& ParamName, float NewValue)
{
    if (CurrentParameters.Contains(ParamName))
    {
        CurrentParameters[ParamName] = NewValue;
    }
}