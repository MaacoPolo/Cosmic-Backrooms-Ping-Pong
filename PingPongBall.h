#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PingPongBall.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class BACKROOMSPINGPONG_API APingPongBall : public AActor
{
    GENERATED_BODY()

public:
    APingPongBall();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<USphereComponent> BallCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UStaticMeshComponent> BallVisual;

    UFUNCTION(BlueprintCallable, Category="Ping Pong")
    void Launch(FVector Direction, float Strength, FVector ExtraVelocity = FVector::ZeroVector);
};
