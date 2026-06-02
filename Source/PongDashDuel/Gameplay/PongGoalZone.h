// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Core/PongTypes.h"
#include "GameFramework/Actor.h"
#include "PongGoalZone.generated.h"

class UBoxComponent;
class UPrimitiveComponent;

UCLASS()
class PONGDASHDUEL_API APongGoalZone : public AActor
{
	GENERATED_BODY()

public:
	APongGoalZone();

	EPongPlayer GetScoringPlayer() const { return ScoringPlayer; }

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	void ConfigureCollision();

	UFUNCTION()
	void OnGoalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Goal")
	EPongPlayer ScoringPlayer = EPongPlayer::None;
};
