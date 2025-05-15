// Fill out your copyright notice in the Description page of Project Settings.


#include "AIUtilityController.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

void AAIUtilityController::BeginPlay()
{
    Super::BeginPlay();


    /* 
    TArray<FAIParameter> Params;
    Params.Add(FAIParameter(TEXT("Work"), 50.0f));
    Params.Add(FAIParameter(TEXT("Stamina"), 80.0f));
    */




    Parameters.Add({ TEXT("Work"), 0.3f });
    Parameters.Add({ TEXT("Satiety"), 0.5f });
    Parameters.Add({ TEXT("WellBeing"), 0.6f });

    //Parameters = {
    //    FAIParameter(TEXT("Work"), 0.3f),
    //  //  FAIParameter(TEXT("Fatigue"), 40.0f),
    ////    FAIParameter(TEXT("Mood"), 60.0f)
    //};

    //  кривые из папки Content/Curves/


    // Computering curves;
    UCurveFloat* ComputeringWorkCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/AI/Actions/Computering/WorkCurve.WorkCurve"));
    UCurveFloat* ComputeringSatietyCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/AI/Actions/Computering/SatietyCurve.SatietyCurve"));
    UCurveFloat* ComputeringWellBeingCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/AI/Actions/Computering/WellBeingCurve.WellBeingCurve"));

    //Eating curves;
    UCurveFloat* EatingWorkCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/AI/Actions/Eating/WorkCurve.WorkCurve"));
    UCurveFloat* EatingSatietyCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/AI/Actions/Eating/SatietyCurve.SatietyCurve"));
    UCurveFloat* EatingWellBeingCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/AI/Actions/Eating/WellBeingCurve.WellBeingCurve"));

    //Talking curves;
    UCurveFloat* TalkingWorkCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/AI/Actions/Talking/WorkCurve.WorkCurve"));
    UCurveFloat* TalkingSatietyCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/AI/Actions/Talking/SatietyCurve.SatietyCurve"));
    UCurveFloat* TalkingWellBeingCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/AI/Actions/Talking/WellBeingCurve.WellBeingCurve"));

    /*
    UCurveFloat* WorkCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/WorkCurve.WorkCurve"));
    UCurveFloat* WorkCurve1 = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/WorkCurve1.WorkCurve1"));
    UCurveFloat* WorkCurve2 = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/ThirdPerson/Blueprints/WorkCurve2.WorkCurve2"));
    */
    /*
    FAIAction Eating(TEXT("Eating"));
    Eating.ParameterEffects = { {TEXT("Work"), -0.13f} };
    Eating.UtilityCurves = { {TEXT("Work"), WorkCurve1} };

    FAIAction Speaking(TEXT("Speaking"));
    Speaking.ParameterEffects = { {TEXT("Work"), -0.31f}};
    Speaking.UtilityCurves = { {TEXT("Work"), WorkCurve2} };

    FAIAction Computering(TEXT("Computering"));
    Computering.ParameterEffects = { {TEXT("Work"), +0.24f} };
    Computering.UtilityCurves = { {TEXT("Work"), WorkCurve} };
    */

    FAIAction Computering(TEXT("Computering"));
    Computering.ParameterEffects = { {TEXT("Work"), +0.13f},{TEXT("Satiety"), -0.2f},{TEXT("WellBeing"), -0.3f} };
    Computering.UtilityCurves = { {TEXT("Work"), ComputeringWorkCurve},{TEXT("Satiety"), ComputeringSatietyCurve} ,{TEXT("WellBeing"), ComputeringWellBeingCurve} };
   
    FAIAction Eating(TEXT("Eating"));
    Eating.ParameterEffects = { {TEXT("Work"), -0.13f},{TEXT("Satiety"), 0.2f},{TEXT("WellBeing"), 0.1f} };
    Eating.UtilityCurves = { {TEXT("Work"), EatingWorkCurve},{TEXT("Satiety"), EatingSatietyCurve} ,{TEXT("WellBeing"), EatingWellBeingCurve} };

    FAIAction Speaking(TEXT("Speaking"));
    Speaking.ParameterEffects = { {TEXT("Work"), -0.15f},{TEXT("Satiety"), 0.12f},{TEXT("WellBeing"), 0.3f} };
    Speaking.UtilityCurves = { {TEXT("Work"), TalkingWorkCurve},{TEXT("Satiety"), TalkingSatietyCurve} ,{TEXT("WellBeing"), TalkingWellBeingCurve} };

    Actions = { Computering, Eating, Speaking };



    for (const FAIParameter& Param : Parameters)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);

    }

    GetWorldTimerManager().SetTimer(UtilityEvaluationTimer, this, &AAIUtilityController::EvaluateAndAct, 2.0f, true);




}

void AAIUtilityController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AAIUtilityController::EvaluateAndAct()
{
    float BestUtility = -FLT_MAX;
    FAIAction* BestAction = nullptr;


    for (FAIAction& Action : Actions)
    {

        if (Action.UtilityCurves.Num() == 0)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("ERROR"));

        }

        float Utility = Action.CalculateUtility(Parameters);
        

        FString DebugMessage = FString::Printf(TEXT("Action [%s] has utility %.2f"), *Action.Name, Utility);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);


        if (Utility > BestUtility)
        {
            BestUtility = Utility;
            BestAction = &Action;
        }
    }

    if (BestAction)
    {

        UE_LOG(LogTemp, Warning, TEXT("✅ CHOSEN ACTION: %s (Utility: %.2f)"), *BestAction->Name, BestUtility);
        BestAction->ApplyEffects(Parameters);
        BestAction->Execute();

        // ⬆️ Показываем текст над ИИ (например, если это Pawn)
        APawn* ControlledPawn = GetPawn();
        if (ControlledPawn)
        {
            DrawDebugString(GetWorld(), ControlledPawn->GetActorLocation() + FVector(0, 0, 100),
                FString::Printf(TEXT("Action: %s"), *BestAction->Name),
                nullptr, FColor::Yellow, 2.0f, true);


        }
    }
}