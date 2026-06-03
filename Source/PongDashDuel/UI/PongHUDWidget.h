// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Core/PongTypes.h"
#include "Blueprint/UserWidget.h"
#include "PongHUDWidget.generated.h"

class UTextBlock;
class UPanelWidget;
class UWidgetAnimation;
class UPongPowerUpDataAsset;
class UPongPowerUpEffect;
class UPongPowerUpItemWidget;

USTRUCT()
struct FPongPowerUpWidgetEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UPongPowerUpEffect> Effect;

	UPROPERTY()
	TObjectPtr<UPongPowerUpItemWidget> Widget;
};

UCLASS()
class PONGDASHDUEL_API UPongHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetScore(int32 Player1Score, int32 Player2Score);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowWinner(EPongPlayer Winner);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void HideWinner();

	void AddPowerUp(EPongPlayer TargetPlayer, UPongPowerUpEffect* Effect, const UPongPowerUpDataAsset* PowerUpData, float TotalDuration);
	void RefreshPowerUp(UPongPowerUpEffect* Effect, float TotalDuration);
	void RemovePowerUp(UPongPowerUpEffect* Effect);
	void UpdatePowerUpProgress(UPongPowerUpEffect* Effect, float RemainingTime);

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ScoreText;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> ScorePopAnimation;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WinnerText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RestartText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> SbPower1;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> SbPower2;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUp")
	TSubclassOf<UPongPowerUpItemWidget> PowerUpItemWidgetClass;

private:
	UPanelWidget* GetPowerUpPanel(EPongPlayer TargetPlayer) const;
	UPongPowerUpItemWidget* FindPowerUpWidget(UPongPowerUpEffect* Effect) const;

	UPROPERTY()
	TArray<FPongPowerUpWidgetEntry> PowerUpWidgetEntries;

	int32 CurrentPlayer1Score = 0;
	int32 CurrentPlayer2Score = 0;
	bool bHasScore = false;
};
