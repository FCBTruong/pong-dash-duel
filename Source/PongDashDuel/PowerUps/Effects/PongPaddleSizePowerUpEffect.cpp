// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongPaddleSizePowerUpEffect.h"

#include "../../Core/PongGameMode.h"
#include "../../Gameplay/PongPaddle.h"

void UPongPaddleSizePowerUpEffect::Apply(APongGameMode* GameMode, EPongPlayer TargetPlayer)
{
	if (GameMode)
	{
		if (APongPaddle* Paddle = GameMode->GetPaddle(TargetPlayer))
		{
			Paddle->SetPaddleSizeMultiplier(SizeMultiplier);
		}
	}
}

void UPongPaddleSizePowerUpEffect::Remove(APongGameMode* GameMode, EPongPlayer TargetPlayer)
{
	if (GameMode)
	{
		if (APongPaddle* Paddle = GameMode->GetPaddle(TargetPlayer))
		{
			Paddle->SetPaddleSizeMultiplier(1.0f);
		}
	}
}
