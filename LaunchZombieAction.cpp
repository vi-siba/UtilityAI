#include "LaunchZombieAction.h"
#include "GameFramework/Character.h"

#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

ULaunchZombieAction::ULaunchZombieAction()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void ULaunchZombieAction::LaunchSelf()
{
    const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();

    if (!ActorInfo)
    {
        EndAbility(CurrentSpecHandle, ActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    AActor* OwnerActor = ActorInfo->OwnerActor.Get();
    ACharacter* Character = Cast<ACharacter>(OwnerActor);
    if (Character)
    {
        FRotator ControlRotation = Character->GetControlRotation();
        FVector LaunchDirection = ControlRotation.Vector();

        Character->LaunchCharacter(LaunchDirection * LaunchStrength, true, true);
    }
}

/*
ACharacter* ULaunchZombieAction::GetOwnedActor();
{
    const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();

    if (!ActorInfo)
    {
        EndAbility(CurrentSpecHandle, ActorInfo, CurrentActivationInfo, true, false);
        return nullptr;
    }

    AActor* OwnerActor = ActorInfo->OwnerActor.Get();
    ACharacter* Character = Cast<ACharacter>(OwnerActor);
    
    if (!Character)
        return nullptr;
    
    return Character;
}
*/



/*
TArray<FVector> ULaunchZombieAction::RaisedPointsArray(TArray<FVector> PointsArray)
{
    for (i = 0, i < (sizeof(PointsArray) / sizeof(PointsArray[0])), i++)
    {
        PointsArray[i].z += RaiseValue;
    }

    return PointsArray;
}
*/

/*

bool UAIBaseAction::ObstaclesAlongWay(FVector Destination)
{
    // to another function
    const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();

    if (!ActorInfo)
    {
        EndAbility(CurrentSpecHandle, ActorInfo, CurrentActivationInfo, true, false);
        return true;
    }

    AActor* OwnerActor = ActorInfo->OwnerActor.Get();
    ACharacter* Character = Cast<ACharacter>(OwnerActor);

    //

    TArray<FVector> PathPoints;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys)
        return true;

    UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(GetWorld(), StartActor->GetActorLocation(), Destination);
    if (NavPath && NavPath->IsValid())
    {
        PathPoints = NavPath->PathPoints;
    }

    int numberOfPoints = sizeof(PathPoints) / sizeof(PathPoints[0]);

    float Distance = FVector::Dist(PathPoints[numberOfPoints - 1], Destination);
    if (Distance < MinimumDistanceToTarget)
        return false;
    else
        return true;
}

*/