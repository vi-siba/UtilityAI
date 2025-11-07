#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "LaunchSelf.generated.h"

UCLASS()
class UTILITYAI_API ULaunchSelf : public UGameplayAbility
{
    GENERATED_BODY()

public:
    ULaunchSelf();

    UFUNCTION(BlueprintCallable, Category = "Ability")
    void LaunchSelf(AActor* ClosestActor);
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Launch")
    TSubclassOf<AActor> ActorClass;


protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Launch")
    float LaunchStrength = 1500.f;

    ACharacter* GetOwnedActor();

private:

    void Function();

    //TSubclassOf<AActor> BuildingClass;
    //TArray<FVector> RaisedPointsArray(TArray<FVector> PointsArray);
};
