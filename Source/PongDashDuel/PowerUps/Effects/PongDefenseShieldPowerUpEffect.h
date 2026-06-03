// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../PongPowerUpEffect.h"
#include "PongDefenseShieldPowerUpEffect.generated.h"

UCLASS()
class PONGDASHDUEL_API UPongDefenseShieldPowerUpEffect : public UPongPowerUpEffect
{
	GENERATED_BODY()

public:
	virtual void Apply(APongGameMode* GameMode, EPongPlayer TargetPlayer) override;
	virtual void Remove(APongGameMode* GameMode, EPongPlayer TargetPlayer) override;
};
