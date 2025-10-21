#include "LaunchZombieAction.h"
#include "GameFramework/Character.h"

ULaunchZombieAction::ULaunchZombieAction()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void ULaunchZombieAction::Launch()
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