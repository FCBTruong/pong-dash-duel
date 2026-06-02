// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongGameMode.h"

#include "Blueprint/UserWidget.h"
#include "PongPlayerController.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "../Core/PongGameDataAsset.h"
#include "../Gameplay/PongBall.h"
#include "../Gameplay/PongPaddle.h"
#include "../UI/PongHUDWidget.h"

APongGameMode::APongGameMode()
{
	PlayerControllerClass = APongPlayerController::StaticClass();
	DefaultPawnClass = nullptr;
}

void APongGameMode::BeginPlay()
{
	Super::BeginPlay();

	Player1Score = 0;
	Player2Score = 0;
	LastScoringPlayer = EPongPlayer::Player2;

	CacheGameplayActors();
	CreateHUD();
	ResetRound();
}

void APongGameMode::ScorePoint(EPongPlayer ScoringPlayer)
{
	if (MatchState != EPongMatchState::Playing || ScoringPlayer == EPongPlayer::None)
	{
		return;
	}

	if (ScoringPlayer == EPongPlayer::Player1)
	{
		++Player1Score;
	}
	else
	{
		++Player2Score;
	}

	LastScoringPlayer = ScoringPlayer;
	UpdateHUD();

	if (Ball)
	{
		PlayGoalFeedback();
		Ball->HideBall();
	}

	if (Player1Score >= WinningScore)
	{
		EndMatch(EPongPlayer::Player1);
		return;
	}

	if (Player2Score >= WinningScore)
	{
		EndMatch(EPongPlayer::Player2);
		return;
	}

	MatchState = EPongMatchState::WaitingToStart;

	FTimerHandle ResetRoundTimerHandle;
	GetWorldTimerManager().SetTimer(ResetRoundTimerHandle, this, &APongGameMode::ResetRound, RoundResetDelay, false);
}

void APongGameMode::ResetRound()
{
	MatchState = EPongMatchState::Playing;
	bWaitingForServeInput = true;

	if (Player1Paddle)
	{
		Player1Paddle->ResetPaddle();
	}

	if (Player2Paddle)
	{
		Player2Paddle->ResetPaddle();
	}

	if (Ball)
	{
		Ball->ResetBall();
	}

	if (HUDWidget)
	{
		HUDWidget->HideWinner();
	}

	UpdateHUD();
}

void APongGameMode::LaunchRoundBall()
{
	if (MatchState != EPongMatchState::Playing || !Ball || !bWaitingForServeInput)
	{
		return;
	}

	bWaitingForServeInput = false;
	const float LaunchDirection = LastScoringPlayer == EPongPlayer::Player1 ? -1.0f : 1.0f;
	Ball->Launch(LaunchDirection);
}

void APongGameMode::NotifyPlayerMoveInput(float Value)
{
	if (bWaitingForServeInput && !FMath::IsNearlyZero(Value))
	{
		LaunchRoundBall();
	}
}

void APongGameMode::EndMatch(EPongPlayer Winner)
{
	MatchState = EPongMatchState::MatchEnded;

	if (Ball)
	{
		Ball->HideBall();
	}
	bWaitingForServeInput = false;

	if (HUDWidget)
	{
		HUDWidget->ShowWinner(Winner);
	}

	UpdateHUD();
}

void APongGameMode::RestartMatch()
{
	Player1Score = 0;
	Player2Score = 0;
	LastScoringPlayer = EPongPlayer::Player2;
	ResetRound();
}

void APongGameMode::UpdateHUD()
{
	if (HUDWidget)
	{
		HUDWidget->SetScore(Player1Score, Player2Score);
	}
}

void APongGameMode::PlayGoalFeedback()
{
	if (!Ball || !GameData)
	{
		return;
	}

	const FVector FeedbackLocation = Ball->GetActorLocation();

	if (GameData->GoalExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, GameData->GoalExplosionEffect, FeedbackLocation);
	}

	if (GameData->GoalSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, GameData->GoalSound, FeedbackLocation);
	}

}

APongPaddle* APongGameMode::GetPaddle(EPongPlayer Player) const
{
	if (Player == EPongPlayer::Player1)
	{
		return Player1Paddle;
	}

	if (Player == EPongPlayer::Player2)
	{
		return Player2Paddle;
	}

	return nullptr;
}

void APongGameMode::CacheGameplayActors()
{
	Ball = nullptr;
	Player1Paddle = nullptr;
	Player2Paddle = nullptr;

	for (TActorIterator<APongBall> It(GetWorld()); It; ++It)
	{
		Ball = *It;
		break;
	}

	for (TActorIterator<APongPaddle> It(GetWorld()); It; ++It)
	{
		APongPaddle* Paddle = *It;
		if (Paddle->GetOwningPlayer() == EPongPlayer::Player1)
		{
			Player1Paddle = Paddle;
		}
		else if (Paddle->GetOwningPlayer() == EPongPlayer::Player2)
		{
			Player2Paddle = Paddle;
		}
	}
}

void APongGameMode::CreateHUD()
{
	if (!HUDWidgetClass)
	{
		return;
	}

	HUDWidget = CreateWidget<UPongHUDWidget>(GetWorld(), HUDWidgetClass);
	if (HUDWidget)
	{
		HUDWidget->AddToViewport();
	}
}
