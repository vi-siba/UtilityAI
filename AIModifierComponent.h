#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIModifierComponent.generated.h"

class UAIBaseAction;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UTILITYAI_API UAIModifierComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAIModifierComponent();

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    void AddActionModifier(FName ActionName, float ModifierValue);

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    void RemoveActionModifier(FName ActionName);

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    void ApplyModifiers(const TArray<UAIBaseAction*>& Actions);

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    void ClearAllModifiers();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Utility AI")
    TMap<FName, float> ActionModifiers;
};