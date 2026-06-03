// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongPowerUpEffect.h"

void UPongPowerUpEffect::Initialize(const UPongPowerUpDataAsset* InData)
{
	Data = const_cast<UPongPowerUpDataAsset*>(InData);
}

void UPongPowerUpEffect::Apply(APongGameMode* GameMode, EPongPlayer TargetPlayer)
{
}

void UPongPowerUpEffect::Remove(APongGameMode* GameMode, EPongPlayer TargetPlayer)
{
}
