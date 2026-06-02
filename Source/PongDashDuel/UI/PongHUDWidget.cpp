// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongHUDWidget.h"

#include "Components/TextBlock.h"

void UPongHUDWidget::SetScore(int32 Player1Score, int32 Player2Score)
{
	const bool bScoreChanged = bHasScore && (Player1Score != CurrentPlayer1Score || Player2Score != CurrentPlayer2Score);

	if (ScoreText)
	{
		ScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d - %d"), Player1Score, Player2Score)));
	}

	if (bScoreChanged && ScorePopAnimation)
	{
		PlayAnimation(ScorePopAnimation);
	}

	CurrentPlayer1Score = Player1Score;
	CurrentPlayer2Score = Player2Score;
	bHasScore = true;
}

void UPongHUDWidget::ShowWinner(EPongPlayer Winner)
{
	if (WinnerText)
	{
		const FString WinnerLabel = Winner == EPongPlayer::Player1 ? TEXT("Player 1 Wins") : TEXT("Player 2 Wins");
		WinnerText->SetText(FText::FromString(WinnerLabel));
		WinnerText->SetVisibility(ESlateVisibility::Visible);
	}

	if (RestartText)
	{
		RestartText->SetText(FText::FromString(TEXT("Press R to Restart")));
		RestartText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPongHUDWidget::HideWinner()
{
	if (WinnerText)
	{
		WinnerText->SetVisibility(ESlateVisibility::Hidden);
	}

	if (RestartText)
	{
		RestartText->SetVisibility(ESlateVisibility::Hidden);
	}
}
