// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongPowerUpItemWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
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
	TotalDuration = FMath::Max(RemainingTime, 0.0f);

	if (UWorld* World = GetWorld())
	{
		EndTime = World->GetTimeSeconds() + TotalDuration;
	}
	else
	{
		EndTime = 0.0f;
	}

	UpdateCountdownDisplay();
	StartOrStopCountdown();
}

void UPongPowerUpItemWidget::NativeDestruct()

{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CountdownTimerHandle);
	}

	Super::NativeDestruct();
}

void UPongPowerUpItemWidget::UpdateCountdownDisplay()
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

	const UWorld* World = GetWorld();
	const float RemainingTime = World ? FMath::Max(EndTime - World->GetTimeSeconds(), 0.0f) : TotalDuration;
	TimeLb->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), RemainingTime)));
}

void UPongPowerUpItemWidget::StartOrStopCountdown()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(CountdownTimerHandle);

	if (TotalDuration > 0.0f)
	{
		World->GetTimerManager().SetTimer(CountdownTimerHandle, this, &UPongPowerUpItemWidget::HandleCountdownTick, 0.05f, true);
	}
}

void UPongPowerUpItemWidget::HandleCountdownTick()
{
	UpdateCountdownDisplay();

	if (UWorld* World = GetWorld())
	{
		if (EndTime <= World->GetTimeSeconds())
		{
			World->GetTimerManager().ClearTimer(CountdownTimerHandle);
		}
	}
}
