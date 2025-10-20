#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIBaseAction.h"
#include "AIModifierComponent.h"

//#include "AIModifierComponent.h"
#include "AIUtilityController.generated.h"

UCLASS()
class UTILITYAI_API AAIUtilityController : public AAIController
{
    GENERATED_BODY()

public:
    AAIUtilityController(const FObjectInitializer& ObjectInitializer);
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility AI")
    TMap<FName, float> CurrentParameters;
    void SetModifierComponent(UAIModifierComponent* ModifierComp);

protected:
    UPROPERTY()
    UAIModifierComponent* ModifierComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility AI")
    TArray<TSubclassOf<UAIBaseAction>> ActionClasses;

    /* UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utility AI")
     UAIModifierComponent* ModifierComponent;*/

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    float FindClosestActorOfClass(TSubclassOf<AActor> ActorClass);

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    void SetParamNewValue(const FName& ParameterName, float NewValue);

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    void EvaluateAndAct();



private:
    void CreateAndInitializeActions(TArray<UAIBaseAction*>& OutActions);

    FTimerHandle UtilityEvaluationTimer;
};