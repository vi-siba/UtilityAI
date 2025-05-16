#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIBaseAction.h"
#include "AIModifierComponent.h"
#include "AIUtilityController.generated.h"

UCLASS()
class UTILITYAI_API AAIUtilityController : public AAIController
{
    GENERATED_BODY()

public:
    AAIUtilityController(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility AI")
    TMap<FName, float> CurrentParameters;


protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility AI")
    TArray<TSubclassOf<UAIBaseAction>> ActionClasses;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utility AI")
    UAIModifierComponent* ModifierComponent;

    void EvaluateAndAct();
    void InitializeActions();

private:
    FTimerHandle UtilityEvaluationTimer;
};