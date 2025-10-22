#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIModifierComponent.generated.h"

USTRUCT(BlueprintType)
struct FCurveModifierMap
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility AI")
    TMap<FName, float> CurveModifiers;
};


class UAIBaseAction;

//UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
UCLASS(Blueprintable)
class UTILITYAI_API UAIModifierComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAIModifierComponent();

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    void AddCurveModifier(FName ActionName, FName CurveName, float ModifierValue);

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    void RemoveCurveModifier(FName ActionName, FName CurveName);

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    void ApplyModifiers(const TArray<UAIBaseAction*>& Actions);

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    void ClearAllModifiers();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Utility AI")
    TMap<FName, FCurveModifierMap> CurveModifiers;

    UFUNCTION(BlueprintCallable, Category = "Utility AI")
    void ModifyCurveUtility(FName ActionName, FName CurveName, float DeltaModifier);

protected:
};
