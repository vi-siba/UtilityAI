#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/NoExportTypes.h"
#include "Curves/CurveFloat.h"

#include "HAL/CriticalSection.h"
#include "NPCSpawner.generated.h"


UCLASS(Blueprintable)
class UTILITYAI_API ANPCSpawner : public AActor
{
    GENERATED_BODY()

public:

    ANPCSpawner();

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
    TMap<FName, UCurveFloat*> UtilityCurves;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility AI")
    TMap<FName, float> CurrentParameters;


private:

};
