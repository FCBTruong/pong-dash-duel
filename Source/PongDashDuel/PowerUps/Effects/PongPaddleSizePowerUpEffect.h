// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../PongPowerUpEffect.h"
#include "PongPaddleSizePowerUpEffect.generated.h"

UCLASS()
class PONGDASHDUEL_API UPongPaddleSizePowerUpEffect : public UPongPowerUpEffect
{
	GENERATED_BODY()

public:
	virtual void Apply(APongGameMode* GameMode, EPongPlayer TargetPlayer) override;
	virtual void Remove(APongGameMode* GameMode, EPongPlayer TargetPlayer) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Paddle")
	float SizeMultiplier = 1.5f;
};
