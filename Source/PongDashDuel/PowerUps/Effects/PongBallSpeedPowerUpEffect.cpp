// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongBallSpeedPowerUpEffect.h"

#include "../../Core/PongGameMode.h"
#include "../../Gameplay/PongBall.h"
#include "../../Gameplay/PongPaddle.h"

void UPongBallSpeedPowerUpEffect::Apply(APongGameMode* GameMode, EPongPlayer TargetPlayer)
{
	if (GameMode)
	{
		if (APongPaddle* Paddle = GameMode->GetPaddle(TargetPlayer))
		{
			Paddle->SetBallHitSpeedMultiplier(SpeedMultiplier);
		}

		if (APongBall* Ball = GameMode->GetBall())
		{
			if (Ball->GetLastHitPlayer() == TargetPlayer)
			{
				Ball->ApplyPaddleHitSpeedMultiplier(SpeedMultiplier);
			}
		}
	}
}

void UPongBallSpeedPowerUpEffect::Remove(APongGameMode* GameMode, EPongPlayer TargetPlayer)
{
	if (GameMode)
	{
		if (APongPaddle* Paddle = GameMode->GetPaddle(TargetPlayer))
		{
			Paddle->SetBallHitSpeedMultiplier(1.0f);
		}
	}
}
