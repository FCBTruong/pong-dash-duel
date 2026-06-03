// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Core/PongTypes.h"
#include "GameFramework/Actor.h"
#include "PongPaddle.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class PONGDASHDUEL_API APongPaddle : public AActor
{
	GENERATED_BODY()

public:
	APongPaddle();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void SetMoveInput(float Value);
	void ResetPaddle();

	EPongPlayer GetOwningPlayer() const { return OwningPlayer; }
	float GetHalfHeight() const;
	void SetPaddleSizeMultiplier(float NewSizeMultiplier);
	float GetBallHitSpeedMultiplier() const { return BallHitSpeedMultiplier; }
	void SetBallHitSpeedMultiplier(float NewSpeedMultiplier);

protected:
	void ConfigureCollision();
	void Move(float DeltaTime);
	void ClampPosition();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Paddle")
	EPongPlayer OwningPlayer = EPongPlayer::None;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 900.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MinX = -400.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxX = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Paddle")
	float HalfHeight = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Paddle")
	float HalfWidth = 18.0f;

	UPROPERTY(EditAnywhere, Category = "Paddle")
	float HalfDepth = 18.0f;

private:
	FVector InitialLocation = FVector::ZeroVector;
	float MoveInput = 0.0f;
	float BaseHalfHeight = 90.0f;
	float BallHitSpeedMultiplier = 1.0f;
};
