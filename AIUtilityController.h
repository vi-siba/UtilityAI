#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIBaseAction.h"
#include "AIModifierComponent.h"
#include "AIUtilityController.generated.h"

/// <summary>
/// AAUTilityController is a base class for creation of a NPC behavior
/// It tasks are: storing a set of the NPC actions, selecting an action to execute, based on it utility
/// </summary>
UCLASS()
class UTILITYAI_API AAIUtilityController : public AAIController 
{
    GENERATED_BODY()

public:
    AAIUtilityController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility AI")
    TMap<FName, float> CurrentParameters;

    void SetModifierComponent(UAIModifierComponent* ModifierComp);
protected:

    // UAIModifierComponent is a component, which lets to additionaly change actions Utility (as in Real-Time, as in Editor)
    UPROPERTY()
    UAIModifierComponent* ModifierComponent;

    // Array of Actions, available to be executed for the NPC
    // You can create different additional custom arrays and switch between them based on NPC type
    // F.e. create NPC controller for Zombies, and in this class create arrays for Melee zombie, Range zombe, ect.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility AI")
    TArray<TSubclassOf<UAIBaseAction>> ActionClasses;


    //!!! This logic is already in AIActorsInteractions class
    //!!! This code needs to be rewritten
    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    float FindClosestActorOfClass(TSubclassOf<AActor> ActorClass);
    //
    //

    /// <summary>
    /// Update of the owned NPC Parameters
    /// </summary>
    /// <param name="ParameterName"></param>
    /// <param name="NewValue"></param>
    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    void SetParamNewValue(const FName& ParameterName, float NewValue);

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    void EvaluateAndAct();

    UPROPERTY()
    TArray<UAIBaseAction*> Actions;

private:
    void CreateAndInitializeActions(TArray<UAIBaseAction*>& OutActions);

    /// <summary>
    /// Timer to restart checking for best option
    /// </summary>
    FTimerHandle UtilityEvaluationTimer;

    
};