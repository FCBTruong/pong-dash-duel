// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongPowerUpItemWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "../PowerUps/PongPowerUpDataAsset.h"

void UPongPowerUpItemWidget::InitializePowerUp(const UPongPowerUpDataAsset* PowerUpData, float InTotalDuration)
{
	TotalDuration = InTotalDuration;

	if (Icon && PowerUpData && PowerUpData->IconTexture)
	{
		Icon->SetBrushFromTexture(PowerUpData->IconTexture, true);
	}

	SetRemainingTime(TotalDuration);
}

void UPongPowerUpItemWidget::SetRemainingTime(float RemainingTime)
{
	if (!TimeLb)
	{
		return;
	}

	if (TotalDuration <= 0.0f)
	{
		TimeLb->SetText(FText::FromString(TEXT("--")));
		return;
	}

	TimeLb->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), FMath::Max(RemainingTime, 0.0f))));
}
