// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PongDangerArea.generated.h"

class USphereComponent;
class UPrimitiveComponent;

UCLASS()
class PONGDASHDUEL_API APongDangerArea : public AActor
{
	GENERATED_BODY()

public:
	APongDangerArea();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	void ConfigureCollision();
	UFUNCTION()
	void OnDangerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDangerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Danger|Shape", meta = (ClampMin = "1.0", UIMin = "1.0"))
	float AreaRadius = 80.0f;

	UPROPERTY(EditAnywhere, Category = "Danger")
	float SlowMultiplier = 0.45f;

	UPROPERTY(EditAnywhere, Category = "Danger")
	float ExitSpeedMultiplier = 1.35f;

	UPROPERTY(EditAnywhere, Category = "Danger")
	float ExitRandomAngleDegrees = 30.0f;
};
