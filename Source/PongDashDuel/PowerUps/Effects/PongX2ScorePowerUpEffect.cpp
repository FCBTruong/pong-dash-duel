// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongX2ScorePowerUpEffect.h"

#include "../../Core/PongGameMode.h"

void UPongX2ScorePowerUpEffect::Apply(APongGameMode* GameMode, EPongPlayer TargetPlayer)
{
	if (GameMode)
	{
		GameMode->SetScoreMultiplier(TargetPlayer, ScoreMultiplier);
	}
}

void UPongX2ScorePowerUpEffect::Remove(APongGameMode* GameMode, EPongPlayer TargetPlayer)
{
	if (GameMode)
	{
		GameMode->SetScoreMultiplier(TargetPlayer, 1);
	}
}
