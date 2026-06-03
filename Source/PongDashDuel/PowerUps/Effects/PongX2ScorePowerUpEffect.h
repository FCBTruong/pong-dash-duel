// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../PongPowerUpEffect.h"
#include "PongX2ScorePowerUpEffect.generated.h"

UCLASS()
class PONGDASHDUEL_API UPongX2ScorePowerUpEffect : public UPongPowerUpEffect
{
	GENERATED_BODY()

public:
	virtual void Apply(APongGameMode* GameMode, EPongPlayer TargetPlayer) override;
	virtual void Remove(APongGameMode* GameMode, EPongPlayer TargetPlayer) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Score")
	int32 ScoreMultiplier = 2;
};
