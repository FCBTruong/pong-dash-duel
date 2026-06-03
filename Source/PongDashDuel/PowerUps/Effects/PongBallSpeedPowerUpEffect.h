// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../PongPowerUpEffect.h"
#include "PongBallSpeedPowerUpEffect.generated.h"

UCLASS()
class PONGDASHDUEL_API UPongBallSpeedPowerUpEffect : public UPongPowerUpEffect
{
	GENERATED_BODY()

public:
	virtual void Apply(APongGameMode* GameMode, EPongPlayer TargetPlayer) override;
	virtual void Remove(APongGameMode* GameMode, EPongPlayer TargetPlayer) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Ball")
	float SpeedMultiplier = 1.8f;
};
