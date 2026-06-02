// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PongTypes.h"
#include "GameFramework/GameModeBase.h"
#include "PongGameMode.generated.h"

class APongBall;
class APongPaddle;
class UPongGameDataAsset;
class UPongHUDWidget;

UCLASS()
class PONGDASHDUEL_API APongGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APongGameMode();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Rules")
	void ScorePoint(EPongPlayer ScoringPlayer);

	UFUNCTION(BlueprintCallable, Category = "Rules")
	void ResetRound();

	void LaunchRoundBall();
	void NotifyPlayerMoveInput(float Value);

	UFUNCTION(BlueprintCallable, Category = "Rules")
	void EndMatch(EPongPlayer Winner);

	UFUNCTION(BlueprintCallable, Category = "Rules")
	void RestartMatch();

	void UpdateHUD();

	APongPaddle* GetPaddle(EPongPlayer Player) const;
	UPongGameDataAsset* GetGameData() const { return GameData; }
	EPongMatchState GetMatchState() const { return MatchState; }

protected:
	void CacheGameplayActors();
	void CreateHUD();
	void PlayGoalFeedback();

	UPROPERTY(EditAnywhere, Category = "Rules")
	int32 WinningScore = 7;

	UPROPERTY(EditAnywhere, Category = "Rules")
	float RoundResetDelay = 0.35f;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPongHUDWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TObjectPtr<UPongGameDataAsset> GameData;

	UPROPERTY(BlueprintReadOnly, Category = "Rules")
	int32 Player1Score = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Rules")
	int32 Player2Score = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Rules")
	EPongMatchState MatchState = EPongMatchState::WaitingToStart;

private:
	UPROPERTY()
	TObjectPtr<APongBall> Ball;

	UPROPERTY()
	TObjectPtr<APongPaddle> Player1Paddle;

	UPROPERTY()
	TObjectPtr<APongPaddle> Player2Paddle;

	UPROPERTY()
	TObjectPtr<UPongHUDWidget> HUDWidget;

	EPongPlayer LastScoringPlayer = EPongPlayer::Player2;
	bool bWaitingForServeInput = false;
};
