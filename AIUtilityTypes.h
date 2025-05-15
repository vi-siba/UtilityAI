// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Curves/CurveFloat.h"
#include "CoreMinimal.h"

struct FAIParameter
{
    FString Name;
    float Value;
    FAIParameter(const FString& InName, float InValue): Name(InName), Value(InValue)
    {}
};

struct FAIAction
{
    FString Name;
    TMap<FString, float> ParameterEffects;
    TMap<FString, UCurveFloat*> UtilityCurves;
    
    FAIAction(const FString& InName) : Name(InName) {}

    float CalculateUtility(const TArray<FAIParameter>& Params) const
    {
        float TotalUtility = 0.0f;
        for (const FAIParameter& Param : Params)
        {
            if (UCurveFloat* Curve = UtilityCurves.FindRef(Param.Name))
            {
                float Utility = Curve->GetFloatValue(Param.Value);
                TotalUtility += Utility;

                FString DebugMessage = FString::Printf(TEXT("%s: ParamValue=%.1f → Utility=%.2f"),*Param.Name, Param.Value, Utility);
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);
            }
        }
        return TotalUtility;
    }



    void ApplyEffects(TArray<FAIParameter>& Params) const
    {
        for (FAIParameter& Param : Params)
        {
            if (const float* Effect = ParameterEffects.Find(Param.Name))
            {
                Param.Value = FMath::Clamp(Param.Value + *Effect, 0.0f, 100.0f);
            }
        }
    }

    void Execute() const
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
                FString::Printf(TEXT("Executing Action: %s"), *Name));
        }

        // Пример кастомной реакции
        if (Name == "Computering")
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("😴 I am computering..."));
        }
        else if (Name == "Eating")
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("🍔 I am eating..."));
        }
        else if (Name == "Speaking")
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, TEXT("🏃 I am speaking..."));
        }
    }


};