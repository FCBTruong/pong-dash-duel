// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongHUDWidget.h"

#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "PongPowerUpItemWidget.h"

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

void UPongHUDWidget::AddPowerUp(EPongPlayer TargetPlayer, UPongPowerUpEffect* Effect, const UPongPowerUpDataAsset* PowerUpData, float TotalDuration)
{
	if (!Effect || !PowerUpData || !PowerUpItemWidgetClass)
	{
		return;
	}

	if (UPongPowerUpItemWidget* ExistingWidget = FindPowerUpWidget(Effect))
	{
		ExistingWidget->InitializePowerUp(PowerUpData, TotalDuration);
		return;
	}

	UPanelWidget* Panel = GetPowerUpPanel(TargetPlayer);
	if (!Panel)
	{
		return;
	}

	UPongPowerUpItemWidget* ItemWidget = CreateWidget<UPongPowerUpItemWidget>(GetWorld(), PowerUpItemWidgetClass);
	if (!ItemWidget)
	{
		return;
	}

	ItemWidget->InitializePowerUp(PowerUpData, TotalDuration);
	if (TargetPlayer == EPongPlayer::Player2)
	{
		ItemWidget->SetRenderScale(FVector2D(-1.0f, 1.0f));
	}
	Panel->AddChild(ItemWidget);

	FPongPowerUpWidgetEntry& Entry = PowerUpWidgetEntries.AddDefaulted_GetRef();
	Entry.Effect = Effect;
	Entry.Widget = ItemWidget;
}

void UPongHUDWidget::RefreshPowerUp(UPongPowerUpEffect* Effect, float TotalDuration)
{
	if (UPongPowerUpItemWidget* ItemWidget = FindPowerUpWidget(Effect))
	{
		ItemWidget->SetRemainingTime(TotalDuration);
	}
}

void UPongHUDWidget::RemovePowerUp(UPongPowerUpEffect* Effect)
{
	for (int32 Index = PowerUpWidgetEntries.Num() - 1; Index >= 0; --Index)
	{
		if (PowerUpWidgetEntries[Index].Effect == Effect)
		{
			if (PowerUpWidgetEntries[Index].Widget)
			{
				PowerUpWidgetEntries[Index].Widget->RemoveFromParent();
			}

			PowerUpWidgetEntries.RemoveAtSwap(Index);
			return;
		}
	}
}

void UPongHUDWidget::UpdatePowerUpProgress(UPongPowerUpEffect* Effect, float RemainingTime)
{
	if (UPongPowerUpItemWidget* ItemWidget = FindPowerUpWidget(Effect))
	{
		ItemWidget->SetRemainingTime(RemainingTime);
	}
}

UPanelWidget* UPongHUDWidget::GetPowerUpPanel(EPongPlayer TargetPlayer) const
{
	if (TargetPlayer == EPongPlayer::Player1)
	{
		return SbPower1;
	}

	if (TargetPlayer == EPongPlayer::Player2)
	{
		return SbPower2;
	}

	return nullptr;
}

UPongPowerUpItemWidget* UPongHUDWidget::FindPowerUpWidget(UPongPowerUpEffect* Effect) const
{
	for (const FPongPowerUpWidgetEntry& Entry : PowerUpWidgetEntries)
	{
		if (Entry.Effect == Effect)
		{
			return Entry.Widget;
		}
	}

	return nullptr;
}
