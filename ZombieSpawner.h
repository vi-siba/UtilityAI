#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/NoExportTypes.h"
#include "Curves/CurveFloat.h"

#include "HAL/CriticalSection.h"
#include "ZombieSpawner.generated.h"


UCLASS(Blueprintable)
class UTILITYAI_API AZombieSpawner : public AActor
{
    GENERATED_BODY()

public:

    AZombieSpawner();

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    void SpawnNPC();
    
    int NPCQuantity = 5;
    bool SpawnSingle(AActor* SpawnPointActor);

    int AdditionalQuantity();

    float Displacement=300.0f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<AActor> ZombieClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
    TArray<AActor*> SpawnPoints;

    /*
    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    float CalculateUtility(const TMap<FName, float>& Parameters);
    */

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
    TMap<FName, UCurveFloat*> UtilityCurves;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility AI")
    TMap<FName, float> CurrentParameters;


private:

};

    /*
    UAIBaseAction();



    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    virtual void ApplyEffects(TMap<FName, float>& Parameters);

    UFUNCTION(BlueprintNativeEvent, Category = "Utility AI")
    void Execute();

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

    UPROPERTY()
    TMap<FName, float> CurveModifiersMap;

    void SetActionExecuterActor(AActor* ExecuterActor);
    void AddOrUpdateCurveModifier(FName CurveName, float ModifierValue);
    virtual void Execute_Implementation();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
    FName ActionName = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
    TMap<FName, float> ParameterEffects;
   
    UPROPERTY(BlueprintReadWrite, Category = "Utility AI")
    float UtilityModifier = 0.0f;
  
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
    AActor* ActionExecuterActor;

    AActor* ClosestActor;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
    TSubclassOf<AActor> ActorClass;
    

private:
    */

