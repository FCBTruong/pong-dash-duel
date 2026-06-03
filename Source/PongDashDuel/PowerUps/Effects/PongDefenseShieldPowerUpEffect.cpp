// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongDefenseShieldPowerUpEffect.h"

#include "../../Core/PongGameMode.h"
#include "../WorldObjects/PongDefenseShield.h"

void UPongDefenseShieldPowerUpEffect::Apply(APongGameMode* GameMode, EPongPlayer TargetPlayer)
{
	if (GameMode)
	{
		if (APongDefenseShield* Shield = GameMode->GetShield(TargetPlayer))
		{
			Shield->ActivateShield();
		}
	}
}

void UPongDefenseShieldPowerUpEffect::Remove(APongGameMode* GameMode, EPongPlayer TargetPlayer)
{
	if (GameMode)
	{
		if (APongDefenseShield* Shield = GameMode->GetShield(TargetPlayer))
		{
			Shield->DeactivateShield();
		}
	}
}
