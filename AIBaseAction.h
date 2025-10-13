#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Curves/CurveFloat.h"
#include "AIBaseAction.generated.h"

/// <summary>
/// UAIBaseAction is a base class for creation of a NPC actions
/// It tasks are: storing a set of the actions parametres Curves, changins NPS parametres after Action execution
/// </summary>
UCLASS(Blueprintable)
class UTILITYAI_API UAIBaseAction : public UObject
{
    GENERATED_BODY()

public:
    UAIBaseAction();

    /// <summary>
    /// Calculation of Action utility, based on current NPC parameteres
    /// </summary>
    /// <param name="Parameters"></param>
    /// <returns></returns>
    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    virtual float CalculateUtility(const TMap<FName, float>& Parameters);

    /// <summary>
    /// Influence of the actions on NPC parameteres after the Executeion
    /// </summary>
    /// <param name="Parameters"></param>
    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    virtual void ApplyEffects(TMap<FName, float>& Parameters);

    /// <summary>
    /// Base Function for Executiong logic call
    /// </summary>
    UFUNCTION(BlueprintNativeEvent, Category = "Utility AI")
    void Execute();

    virtual void Execute_Implementation();

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    FName GetActionName() const { return ActionName; }

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    float GetUtilityModifier() const { return UtilityModifier; }

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    void SetUtilityModifier(float NewModifier) { UtilityModifier = NewModifier; }

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    virtual TArray<FName> GetRelevantParameters() const;

    UPROPERTY(BlueprintReadWrite, Category = "Utility AI")
    TMap<FName, float> CurveModifiers;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
    TMap<FName, UCurveFloat*> UtilityCurves;

    void AddOrUpdateCurveModifier(FName CurveName, float ModifierValue);

    UPROPERTY()
    TMap<FName, float> CurveModifiersMap;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
    FName ActionName = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
    TMap<FName, float> ParameterEffects;
   
    UPROPERTY(BlueprintReadWrite, Category = "Utility AI")
    float UtilityModifier = 0.0f;
};