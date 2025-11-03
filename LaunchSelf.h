#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "LaunchZombieAction.generated.h"

UCLASS()
class UTILITYAI_API ULaunchSelf : public UGameplayAbility
{
    GENERATED_BODY()

public:
    ULaunchSelf();

    UFUNCTION(BlueprintCallable, Category = "Ability")
    void LaunchSelf(AActor* ClosestActor);
    
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Launch")
    float LaunchStrength = 1500.f;

    ACharacter* GetOwnedActor();

private:

    //TSubclassOf<AActor> BuildingClass;
    //TArray<FVector> RaisedPointsArray(TArray<FVector> PointsArray);
};