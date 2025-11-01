#include "LaunchProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

ALaunchProjectile::ALaunchProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

    SetupProjectileMovement();
}

void ALaunchProjectile::BeginPlay()
{
    Super::BeginPlay();
    
}

void ALaunchProjectile::SetupProjectileMovement()
{
    if (ProjectileMovementComponent)
    {
        ProjectileMovementComponent->UpdatedComponent = RootComponent;
        ProjectileMovementComponent->InitialSpeed = InitialSpeed;
        ProjectileMovementComponent->MaxSpeed = MaxSpeed;
        ProjectileMovementComponent->bRotationFollowsVelocity = bRotationFollowsVelocity;
        ProjectileMovementComponent->ProjectileGravityScale = ProjectileGravityScale;
        ProjectileMovementComponent->bShouldBounce = false;
        ProjectileMovementComponent->bIsHomingProjectile = false;
        ProjectileMovementComponent->SetAutoActivate(false); 
    }
}

void ALaunchProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ALaunchProjectile::SetTarget(AActor* NewTarget)
{
    TargetActor = NewTarget;

    if (TargetActor && ProjectileMovementComponent)
    {
        LaunchTowardsTarget();
    }
}

void ALaunchProjectile::LaunchTowardsTarget()
{
    if (!TargetActor || !ProjectileMovementComponent)
    {
        return;
    }

    FVector StartLocation = GetActorLocation();
    FVector TargetLocation = TargetActor->GetActorLocation();

    FVector LaunchVelocity = CalculateTrajectoryVelocity(StartLocation, TargetLocation, InitialSpeed);

    if (!LaunchVelocity.IsZero())
    {
        ProjectileMovementComponent->Velocity = LaunchVelocity;
        ProjectileMovementComponent->Activate();

    }
}

FVector ALaunchProjectile::CalculateTrajectoryVelocity(FVector StartLoc, FVector TargetLoc, float Speed)
{
    FVector OutVelocity = FVector::ZeroVector;

    bool bSuccess = UGameplayStatics::SuggestProjectileVelocity(
        this,
        OutVelocity,
        StartLoc,
        TargetLoc,
        Speed,
        true, 
        0.0f,  
        GetWorld()->GetGravityZ(), 
        ESuggestProjVelocityTraceOption::DoNotTrace,
        FCollisionResponseParams::DefaultResponseParam,
        TArray<AActor*>(),
        false 
    );

    if (!bSuccess)
    {
        FVector Direction = (TargetLoc - StartLoc).GetSafeNormal();
        OutVelocity = Direction * Speed;

        UE_LOG(LogTemp, Warning, TEXT("Projectile trajectory calculation failed, using fallback direction"));
    }

    return OutVelocity;
}