#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "LaunchZombieAction.generated.h"

UCLASS()
class UTILITYAI_API ULaunchZombieAction : public UGameplayAbility
{
    GENERATED_BODY()

public:
    ULaunchZombieAction();

    UFUNCTION(BlueprintCallable, Category = "Ability")
    void LaunchSelf();


protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Launch")
    float LaunchStrength = 1500.f;

    ACharacter* GetOwnedActor();

    // Check for Obstacles along the AI way to target;
    float RaiseValue=10.0f;
    float AttackRange=100.0f;
    bool ObstaclesAlongWay(FVector Destination);

private:

    //TSubclassOf<AActor> BuildingClass;
    //TArray<FVector> RaisedPointsArray(TArray<FVector> PointsArray);
};