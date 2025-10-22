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

    bool ObstaclesAlongWay(FVector Destination);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Launch")
    float LaunchStrength = 1500.f;

    ACharacter* GetOwnedActor();
    float RaiseValue=10.0f;

private:
    TSubclassOf<AActor> BuildingClass;
    TArray<FVector> RaisedPointsArray(TArray<FVector> PointsArray);
};