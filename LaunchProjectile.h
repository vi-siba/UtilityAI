// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaunchProjectile.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class UTILITYAI_API ALaunchProjectile : public AActor
{
    GENERATED_BODY()

public:
    ALaunchProjectile();

protected:
    virtual void BeginPlay() override;

    /*
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComponent;
    */

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProjectileMovementComponent* ProjectileMovementComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    AActor* TargetActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float InitialSpeed = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float MaxSpeed = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float ProjectileGravityScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    bool bRotationFollowsVelocity = true;

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void SetTarget(AActor* NewTarget);

    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void LaunchTowardsTarget();

    UFUNCTION(BlueprintCallable, Category = "Projectile")
    FVector CalculateTrajectoryVelocity(FVector StartLoc, FVector TargetLoc, float Speed);

private:
    void SetupProjectileMovement();
};