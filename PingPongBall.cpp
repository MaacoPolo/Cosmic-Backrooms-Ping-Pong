#include "PingPongBall.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

APingPongBall::APingPongBall()
{
    PrimaryActorTick.bCanEverTick = false;

    BallCollision = CreateDefaultSubobject<USphereComponent>(TEXT("BallCollision"));
    SetRootComponent(BallCollision);
    BallCollision->InitSphereRadius(2.0f);
    BallCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BallCollision->SetCollisionObjectType(ECC_PhysicsBody);
    BallCollision->SetCollisionResponseToAllChannels(ECR_Block);
    BallCollision->SetSimulatePhysics(true);
    BallCollision->SetEnableGravity(true);
    BallCollision->SetNotifyRigidBodyCollision(true);
    BallCollision->SetLinearDamping(0.08f);
    BallCollision->SetAngularDamping(0.12f);
    BallCollision->SetMassOverrideInKg(NAME_None, 0.0027f, true);
    BallCollision->SetUseCCD(true);

    BallVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallVisual"));
    BallVisual->SetupAttachment(BallCollision);
    BallVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APingPongBall::Launch(FVector Direction, float Strength, FVector ExtraVelocity)
{
    if (!BallCollision) return;

    BallCollision->WakeAllRigidBodies();
    BallCollision->SetPhysicsLinearVelocity(FVector::ZeroVector);
    BallCollision->AddImpulse(Direction.GetSafeNormal() * Strength + ExtraVelocity, NAME_None, true);
}
