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
#include "../PowerUps/WorldObjects/PongDefenseShield.h"
#include "../PowerUps/WorldObjects/PongPowerProjectile.h"
#include "../PowerUps/PongPowerUpComponent.h"
#include "../PowerUps/PongPowerUpItem.h"
#include "../UI/PongHUDWidget.h"

APongGameMode::APongGameMode()
{
	PlayerControllerClass = APongPlayerController::StaticClass();
	DefaultPawnClass = nullptr;
	PowerUpComponent = CreateDefaultSubobject<UPongPowerUpComponent>(TEXT("PowerUpComponent"));
}

void APongGameMode::BeginPlay()
{
	Super::BeginPlay();

	Player1Score = 0;
	Player2Score = 0;
	LastScoringPlayer = EPongPlayer::Player2;

	CacheGameplayActors();
	CreateHUD();
	ResetPaddles();
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
		Player1Score += GetScoreMultiplier(EPongPlayer::Player1);
	}
	else
	{
		Player2Score += GetScoreMultiplier(EPongPlayer::Player2);
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
	bWaitingForServeInput = false;
	GetWorldTimerManager().ClearTimer(BallAppearTimerHandle);

	if (Ball)
	{
		Ball->ResetBallHidden();
		Ball->PlaySpawnFeedback();
	}

	if (HUDWidget)
	{
		HUDWidget->HideWinner();
	}

	UpdateHUD();

	GetWorldTimerManager().SetTimer(BallAppearTimerHandle, this, &APongGameMode::ShowRoundBall, BallAppearDelay, false);
}

void APongGameMode::ShowRoundBall()
{
	if (MatchState != EPongMatchState::Playing || !Ball)
	{
		return;
	}

	Ball->ShowBall();
	bWaitingForServeInput = true;
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
	GetWorldTimerManager().ClearTimer(BallAppearTimerHandle);
	bWaitingForServeInput = false;

	if (HUDWidget)
	{
		HUDWidget->ShowWinner(Winner);
	}

	UpdateHUD();
}

void APongGameMode::RestartMatch()
{
	ClearPowerUpState();
	Player1Score = 0;
	Player2Score = 0;
	Player1ScoreMultiplier = 1;
	Player2ScoreMultiplier = 1;
	LastScoringPlayer = EPongPlayer::Player2;
	ResetPaddles();
	ResetRound();
}

void APongGameMode::ClearPowerUpState()
{
	if (PowerUpComponent)
	{
		PowerUpComponent->ClearAllPowerUps();
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (TActorIterator<APongPowerUpItem> ItemIterator(World); ItemIterator; ++ItemIterator)
	{
		APongPowerUpItem* Item = *ItemIterator;
		if (Item && !Item->IsActorBeingDestroyed())
		{
			Item->Destroy();
		}
	}

	for (TActorIterator<APongPowerProjectile> ProjectileIterator(World); ProjectileIterator; ++ProjectileIterator)
	{
		APongPowerProjectile* Projectile = *ProjectileIterator;
		if (Projectile && !Projectile->IsActorBeingDestroyed())
		{
			Projectile->Destroy();
		}
	}
}

void APongGameMode::UpdateHUD()
{
	if (HUDWidget)
	{
		HUDWidget->SetScore(Player1Score, Player2Score);
	}
}

void APongGameMode::NotifyPowerUpStarted(EPongPlayer TargetPlayer, UPongPowerUpEffect* Effect, const UPongPowerUpDataAsset* PowerUpData, float TotalDuration)
{
	if (HUDWidget)
	{
		HUDWidget->AddPowerUp(TargetPlayer, Effect, PowerUpData, TotalDuration);
	}
}

void APongGameMode::NotifyPowerUpRefreshed(UPongPowerUpEffect* Effect, float TotalDuration)
{
	if (HUDWidget)
	{
		HUDWidget->RefreshPowerUp(Effect, TotalDuration);
	}
}

void APongGameMode::NotifyPowerUpRemoved(UPongPowerUpEffect* Effect)
{
	if (HUDWidget)
	{
		HUDWidget->RemovePowerUp(Effect);
	}
}

void APongGameMode::ResetPaddles()
{
	if (Player1Paddle)
	{
		Player1Paddle->ResetPaddle();
	}

	if (Player2Paddle)
	{
		Player2Paddle->ResetPaddle();
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

APongPaddle* APongGameMode::GetOpponentPaddle(EPongPlayer Player) const
{
	if (Player == EPongPlayer::Player1)
	{
		return Player2Paddle;
	}

	if (Player == EPongPlayer::Player2)
	{
		return Player1Paddle;
	}

	return nullptr;
}

APongDefenseShield* APongGameMode::GetShield(EPongPlayer Player) const
{
	if (Player == EPongPlayer::Player1)
	{
		return Player1Shield;
	}

	if (Player == EPongPlayer::Player2)
	{
		return Player2Shield;
	}

	return nullptr;
}

void APongGameMode::SetScoreMultiplier(EPongPlayer Player, int32 NewMultiplier)
{
	const int32 SafeMultiplier = FMath::Max(NewMultiplier, 1);

	if (Player == EPongPlayer::Player1)
	{
		Player1ScoreMultiplier = SafeMultiplier;
	}
	else if (Player == EPongPlayer::Player2)
	{
		Player2ScoreMultiplier = SafeMultiplier;
	}
}

int32 APongGameMode::GetScoreMultiplier(EPongPlayer Player) const
{
	if (Player == EPongPlayer::Player1)
	{
		return Player1ScoreMultiplier;
	}

	if (Player == EPongPlayer::Player2)
	{
		return Player2ScoreMultiplier;
	}

	return 1;
}

void APongGameMode::CacheGameplayActors()
{
	Ball = nullptr;
	Player1Paddle = nullptr;
	Player2Paddle = nullptr;
	Player1Shield = nullptr;
	Player2Shield = nullptr;

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

	for (TActorIterator<APongDefenseShield> It(GetWorld()); It; ++It)
	{
		APongDefenseShield* Shield = *It;
		if (Shield->GetOwningPlayer() == EPongPlayer::Player1)
		{
			Player1Shield = Shield;
		}
		else if (Shield->GetOwningPlayer() == EPongPlayer::Player2)
		{
			Player2Shield = Shield;
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
