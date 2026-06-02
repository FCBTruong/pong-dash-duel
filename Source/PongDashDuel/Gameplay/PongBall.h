// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PongBall.generated.h"

class APongPaddle;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class PONGDASHDUEL_API APongBall : public AActor
{
	GENERATED_BODY()

public:
	APongBall();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void Launch(float DirectionX = 1.0f);
	void Stop();
	void ResetBall();
	void HideBall();
	void ShowBall();
	void BounceFromWall(const FVector& ImpactLocation, const FVector& ImpactNormal);
	void BounceFromPaddle(const APongPaddle* Paddle, const FVector& ImpactLocation, const FVector& ImpactNormal);

	bool IsMoving() const { return !Velocity.IsNearlyZero(); }

protected:
	void ConfigureCollision();
	void ResetTrail();
	void PlayBounceFeedback(const FVector& Location, const FVector& Normal);
	void Move(float DeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Ball")
	float InitialSpeed = 700.0f;

	UPROPERTY(EditAnywhere, Category = "Ball")
	float MaxSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, Category = "Ball")
	float SpeedIncreaseOnPaddleHit = 35.0f;

	UPROPERTY(EditAnywhere, Category = "Ball")
	float BounceXStrength = 900.0f;

private:
	FVector InitialLocation = FVector::ZeroVector;
	FVector Velocity = FVector::ZeroVector;
	float CurrentSpeed = 0.0f;
};
