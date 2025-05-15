// Fill out your copyright notice in the Description page of Project Settings.

//#pragma once
//
//#include "CoreMinimal.h"
//
///**
// * 
// */
//class UTILITYAI_API AIUtilityController
//{
//public:
//	AIUtilityController();
//	~AIUtilityController();
//};



#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIUtilityTypes.h"
#include "AIUtilityController.generated.h"

UCLASS()
class AAIUtilityController : public AAIController
{
    GENERATED_BODY()


public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    TArray<FAIParameter> Parameters;
    TArray<FAIAction> Actions;


    void EvaluateAndAct();

    FTimerHandle UtilityEvaluationTimer;


};