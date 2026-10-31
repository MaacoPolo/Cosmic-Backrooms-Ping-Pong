#include "BackroomsPingPongCharacter.h"
#include "PingPongBall.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EngineUtils.h"

ABackroomsPingPongCharacter::ABackroomsPingPongCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
    PlayerCamera->SetupAttachment(GetRootComponent());
    PlayerCamera->SetRelativeLocation(FVector(-260.f, 0.f, 145.f));
    PlayerCamera->SetRelativeRotation(FRotator(-8.f, 0.f, 0.f));

    PaddlePivot = CreateDefaultSubobject<USceneComponent>(TEXT("PaddlePivot"));
    PaddlePivot->SetupAttachment(GetRootComponent());
    PaddlePivot->SetRelativeLocation(FVector(65.f, 48.f, 75.f));

    PaddleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PaddleMesh"));
    PaddleMesh->SetupAttachment(PaddlePivot);
    PaddleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    PaddleHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("PaddleHitbox"));
    PaddleHitbox->SetupAttachment(PaddlePivot);
    PaddleHitbox->SetBoxExtent(FVector(4.f, 15.f, 20.f));
    PaddleHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PaddleHitbox->SetCollisionObjectType(ECC_WorldDynamic);
    PaddleHitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
    PaddleHitbox->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
    PaddleHitbox->SetGenerateOverlapEvents(false);

    GetCharacterMovement()->MaxWalkSpeed = 270.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 900.f;
}

void ABackroomsPingPongCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (GetMesh())
    {
        USkeletalMesh* ChosenMesh = bPlayAsPig ? PigMesh.Get() : HippoMesh.Get();
        if (ChosenMesh) GetMesh()->SetSkeletalMesh(ChosenMesh);
    }

    PaddleHitbox->OnComponentHit.AddDynamic(
        this, &ABackroomsPingPongCharacter::OnPaddleHit);

    for (TActorIterator<APingPongBall> It(GetWorld()); It; ++It)
    {
        Ball = *It;
        break;
    }

    LastPaddleWorldLocation = PaddlePivot->GetComponentLocation();
}

void ABackroomsPingPongCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    const FVector Current = PaddlePivot->GetComponentLocation();
    if (DeltaSeconds > KINDA_SMALL_NUMBER)
        PaddleVelocity = (Current - LastPaddleWorldLocation) / DeltaSeconds;
    LastPaddleWorldLocation = Current;

    // Keep the paddle inside a comfortable reach in front of the character.
    const FVector2D Offset = PaddlePivot->GetRelativeLocation().IsNearlyZero()
        ? FVector2D::ZeroVector
        : FVector2D(PaddlePivot->GetRelativeLocation().X - 65.f,
                    PaddlePivot->GetRelativeLocation().Y - 48.f);

    FVector Local = PaddlePivot->GetRelativeLocation();
    Local.X = 65.f + FMath::Clamp(Offset.X, -PaddleReach.X, PaddleReach.X);
    Local.Y = 48.f + FMath::Clamp(Offset.Y, -PaddleReach.Y, PaddleReach.Y);
    Local.Z = 75.f + FMath::Clamp(PaddleInput.Y * 18.f, -18.f, 18.f);
    PaddlePivot->SetRelativeLocation(Local);
}

void ABackroomsPingPongCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABackroomsPingPongCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ABackroomsPingPongCharacter::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("PaddleForward"), this, &ABackroomsPingPongCharacter::MovePaddleForward);
    PlayerInputComponent->BindAxis(TEXT("PaddleRight"), this, &ABackroomsPingPongCharacter::MovePaddleRight);
    PlayerInputComponent->BindAction(TEXT("ServeOrSmash"), IE_Pressed, this, &ABackroomsPingPongCharacter::ServeOrSmash);
}

void ABackroomsPingPongCharacter::MoveForward(float Value)
{
    if (FMath::Abs(Value) > 0.05f)
        AddMovementInput(GetActorForwardVector(), Value);
}

void ABackroomsPingPongCharacter::MoveRight(float Value)
{
    if (FMath::Abs(Value) > 0.05f)
        AddMovementInput(GetActorRightVector(), Value);
}

void ABackroomsPingPongCharacter::MovePaddleForward(float Value)
{
    PaddleInput.X = Value;
    FVector Local = PaddlePivot->GetRelativeLocation();
    Local.X += Value * 3.0f;
    Local.X = FMath::Clamp(Local.X, 65.f - PaddleReach.X, 65.f + PaddleReach.X);
    PaddlePivot->SetRelativeLocation(Local);
}

void ABackroomsPingPongCharacter::MovePaddleRight(float Value)
{
    PaddleInput.Y = Value;
    FVector Local = PaddlePivot->GetRelativeLocation();
    Local.Y += Value * 3.0f;
    Local.Y = FMath::Clamp(Local.Y, 48.f - PaddleReach.Y, 48.f + PaddleReach.Y);
    PaddlePivot->SetRelativeLocation(Local);
}

void ABackroomsPingPongCharacter::ServeOrSmash()
{
    if (!Ball) return;

    const float Distance = FVector::Distance(
        PaddleHitbox->GetComponentLocation(), Ball->GetActorLocation());

    if (Distance < 230.f)
    {
        FVector Direction = GetActorForwardVector() * 0.8f
                          + GetActorUpVector() * 0.45f;
        Direction += GetActorRightVector() * PaddleInput.Y * 0.25f;
        Ball->Launch(Direction.GetSafeNormal(), PaddleImpulse);
    }
}

void ABackroomsPingPongCharacter::OnPaddleHit(
    UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse,
    const FHitResult& Hit)
{
    APingPongBall* HitBall = Cast<APingPongBall>(OtherActor);
    if (!HitBall) return;

    FVector Direction = GetActorForwardVector() * 0.75f
                      + GetActorUpVector() * 0.55f
                      + GetActorRightVector() * PaddleInput.Y * 0.2f;

    const FVector SwingBoost = PaddleVelocity.GetClampedToMaxSize(650.f) * 0.35f;
    HitBall->Launch(Direction.GetSafeNormal(), PaddleImpulse, SwingBoost);
}
