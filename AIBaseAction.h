#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Curves/CurveFloat.h"
#include "AIBaseAction.generated.h"

UCLASS(Blueprintable)
class UTILITYAI_API UAIBaseAction : public UObject
{
    GENERATED_BODY()

public:
    UAIBaseAction();

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    virtual float CalculateUtility(const TMap<FName, float>& Parameters);

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    virtual void ApplyEffects(TMap<FName, float>& Parameters);

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


protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
    FName ActionName = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
    TMap<FName, float> ParameterEffects;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
    TMap<FName, UCurveFloat*> UtilityCurves;

    UPROPERTY(BlueprintReadWrite, Category = "Utility AI")
    float UtilityModifier = 0.0f;
};