#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BackroomsPingPongCharacter.generated.h"

class UCameraComponent;
class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;
class APingPongBall;

UCLASS()
class BACKROOMSPINGPONG_API ABackroomsPingPongCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ABackroomsPingPongCharacter();

    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // Assign the appropriate character mesh in the Blueprint child.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Select")
    bool bPlayAsPig = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Select")
    TObjectPtr<USkeletalMesh> HippoMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Select")
    TObjectPtr<USkeletalMesh> PigMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UCameraComponent> PlayerCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<USceneComponent> PaddlePivot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UStaticMeshComponent> PaddleMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UBoxComponent> PaddleHitbox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Paddle", meta=(ClampMin="10.0"))
    float PaddleImpulse = 950.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Paddle")
    FVector2D PaddleReach = FVector2D(75.0f, 55.0f);

protected:
    virtual void BeginPlay() override;

private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void MovePaddleForward(float Value);
    void MovePaddleRight(float Value);
    void ServeOrSmash();
    void OnPaddleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                     UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                     const FHitResult& Hit);

    FVector2D PaddleInput = FVector2D::ZeroVector;
    FVector LastPaddleWorldLocation = FVector::ZeroVector;
    FVector PaddleVelocity = FVector::ZeroVector;
    TObjectPtr<APingPongBall> Ball;
};
