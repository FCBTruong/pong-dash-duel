// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PongTypes.h"
#include "GameFramework/GameModeBase.h"
#include "PongGameMode.generated.h"

class APongBall;
class APongDefenseShield;
class APongPaddle;
class UPongGameDataAsset;
class UPongHUDWidget;
class UPongPowerUpComponent;
class UPongPowerUpDataAsset;
class UPongPowerUpEffect;

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
	void ShowRoundBall();
	void NotifyPlayerMoveInput(float Value);

	UFUNCTION(BlueprintCallable, Category = "Rules")
	void EndMatch(EPongPlayer Winner);

	UFUNCTION(BlueprintCallable, Category = "Rules")
	void RestartMatch();

	void UpdateHUD();
	void NotifyPowerUpStarted(EPongPlayer TargetPlayer, UPongPowerUpEffect* Effect, const UPongPowerUpDataAsset* PowerUpData, float TotalDuration);
	void NotifyPowerUpRefreshed(UPongPowerUpEffect* Effect, float TotalDuration);
	void NotifyPowerUpRemoved(UPongPowerUpEffect* Effect);

	APongPaddle* GetPaddle(EPongPlayer Player) const;
	APongPaddle* GetOpponentPaddle(EPongPlayer Player) const;
	APongBall* GetBall() const { return Ball; }
	APongDefenseShield* GetShield(EPongPlayer Player) const;
	UPongPowerUpComponent* GetPowerUpComponent() const { return PowerUpComponent; }
	UPongGameDataAsset* GetGameData() const { return GameData; }
	EPongMatchState GetMatchState() const { return MatchState; }
	void SetScoreMultiplier(EPongPlayer Player, int32 NewMultiplier);
	int32 GetScoreMultiplier(EPongPlayer Player) const;

protected:
	void CacheGameplayActors();
	void CreateHUD();
	void ResetPaddles();
	void PlayGoalFeedback();
	void ClearPowerUpState();

	UPROPERTY(EditAnywhere, Category = "Rules")
	int32 WinningScore = 7;

	UPROPERTY(EditAnywhere, Category = "Rules")
	float RoundResetDelay = 0.35f;

	UPROPERTY(EditAnywhere, Category = "Rules")
	float BallAppearDelay = 2.0f;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPongHUDWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TObjectPtr<UPongGameDataAsset> GameData;

	UPROPERTY(BlueprintReadOnly, Category = "Rules")
	int32 Player1Score = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Rules")
	int32 Player2Score = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Rules")
	int32 Player1ScoreMultiplier = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Rules")
	int32 Player2ScoreMultiplier = 1;

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
	TObjectPtr<APongDefenseShield> Player1Shield;

	UPROPERTY()
	TObjectPtr<APongDefenseShield> Player2Shield;

	UPROPERTY(VisibleAnywhere, Category = "PowerUp")
	TObjectPtr<UPongPowerUpComponent> PowerUpComponent;

	UPROPERTY()
	TObjectPtr<UPongHUDWidget> HUDWidget;

	EPongPlayer LastScoringPlayer = EPongPlayer::Player2;
	bool bWaitingForServeInput = false;
	FTimerHandle BallAppearTimerHandle;
};
