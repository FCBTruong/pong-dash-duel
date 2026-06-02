// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Core/PongTypes.h"
#include "Blueprint/UserWidget.h"
#include "PongHUDWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;

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

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ScoreText;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> ScorePopAnimation;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WinnerText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RestartText;

private:
	int32 CurrentPlayer1Score = 0;
	int32 CurrentPlayer2Score = 0;
	bool bHasScore = false;
};
