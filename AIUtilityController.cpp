#include "AIUtilityController.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "AIModifierComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "EngineUtils.h"


AAIUtilityController::AAIUtilityController(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
    PrimaryActorTick.bCanEverTick = true;
}

void AAIUtilityController::BeginPlay()
{
    Super::BeginPlay();

    UCrowdFollowingComponent* CrowdFollowingComponent = FindComponentByClass<UCrowdFollowingComponent>();
    if (CrowdFollowingComponent)
    {
        CrowdFollowingComponent->SetCrowdSeparation(true);
        CrowdFollowingComponent->SetCrowdSeparationWeight(50.0f);
        CrowdFollowingComponent->SetCrowdAvoidanceRangeMultiplier(1.1f);
    }

    CreateAndInitializeActions(Actions);

    GetWorldTimerManager().SetTimer(UtilityEvaluationTimer, this, &AAIUtilityController::EvaluateAndAct, 2.0f, true);
}


void AAIUtilityController::EvaluateAndAct()
{
    if (ModifierComponent)
    {
        ModifierComponent->ApplyModifiers(Actions);
    }

    UAIBaseAction* BestAction = nullptr;
    float HighestUtility = -FLT_MAX;

    for (UAIBaseAction* Action : Actions)
    {
        if (!Action || !IsValid(Action))
        {
            UE_LOG(LogTemp, Warning, TEXT("Null or invalid Action pointer in EvaluateAndAct."));
            continue;
        }

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
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid BestAction found."));
    }
}


void AAIUtilityController::CreateAndInitializeActions(TArray<UAIBaseAction*>& OutActions)
{
    for (TSubclassOf<UAIBaseAction> ActionClass : ActionClasses)
    {
        if (!ActionClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid ActionClass in CreateAndInitializeActions."));
            continue;
        }

        UAIBaseAction* NewAction = NewObject<UAIBaseAction>(this, ActionClass);
        if (!NewAction || !IsValid(NewAction))
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to create or invalid NewAction."));
            continue;
        }

        NewAction->SetActionExecuterActor(GetPawn());

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