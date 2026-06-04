// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PongPowerUpItemWidget.generated.h"

class UImage;
class UTextBlock;
class UPongPowerUpDataAsset;

UCLASS()
class PONGDASHDUEL_API UPongPowerUpItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializePowerUp(const UPongPowerUpDataAsset* PowerUpData, float InTotalDuration);
	void SetRemainingTime(float RemainingTime);
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Icon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TimeLb;

private:
	void UpdateCountdownDisplay();
	void StartOrStopCountdown();
	void HandleCountdownTick();

	float TotalDuration = 0.0f;
	float EndTime = 0.0f;
	FTimerHandle CountdownTimerHandle;
};
