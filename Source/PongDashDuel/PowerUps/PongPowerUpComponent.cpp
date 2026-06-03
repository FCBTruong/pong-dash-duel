// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongPowerUpComponent.h"

#include "../Core/PongGameMode.h"
#include "PongPowerUpDataAsset.h"
#include "PongPowerUpEffect.h"
#include "TimerManager.h"

UPongPowerUpComponent::UPongPowerUpComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UPongPowerUpComponent::ApplyPowerUp(const UPongPowerUpDataAsset* PowerUpData, EPongPlayer TargetPlayer, AActor* SourceItem)
{
	if (!PowerUpData || !PowerUpData->EffectClass || TargetPlayer == EPongPlayer::None)
	{
		return false;
	}

	if (FActivePongPowerUp* ExistingPowerUp = FindActivePowerUp(PowerUpData, TargetPlayer))
	{
		switch (PowerUpData->StackPolicy)
		{
		case EPongPowerUpStackPolicy::IgnoreIfActive:
			return false;
		case EPongPowerUpStackPolicy::RefreshDuration:
			RefreshPowerUp(*ExistingPowerUp);
			return true;
		case EPongPowerUpStackPolicy::Replace:
			ExpirePowerUp(ExistingPowerUp->Effect);
			break;
		case EPongPowerUpStackPolicy::Stack:
			ExtendPowerUpDuration(*ExistingPowerUp);
			return true;
		default:
			break;
		}
	}

	return StartPowerUpInstance(PowerUpData, TargetPlayer, SourceItem);
}

void UPongPowerUpComponent::ClearAllPowerUps()
{
	for (int32 Index = ActivePowerUps.Num() - 1; Index >= 0; --Index)
	{
		RemovePowerUpAtIndex(Index);
	}
}

void UPongPowerUpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APongGameMode* GameMode = GetPongGameMode();
	if (!GameMode || !GetWorld())
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	for (const FActivePongPowerUp& ActivePowerUp : ActivePowerUps)
	{
		if (ActivePowerUp.Effect && ActivePowerUp.Data && ActivePowerUp.Data->Duration > 0.0f)
		{
			GameMode->NotifyPowerUpProgress(ActivePowerUp.Effect, FMath::Max(ActivePowerUp.EndTime - CurrentTime, 0.0f));
		}
	}
}

void UPongPowerUpComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearAllPowerUps();
	Super::EndPlay(EndPlayReason);
}

APongGameMode* UPongPowerUpComponent::GetPongGameMode() const
{
	return Cast<APongGameMode>(GetOwner());
}

FActivePongPowerUp* UPongPowerUpComponent::FindActivePowerUp(const UPongPowerUpDataAsset* PowerUpData, EPongPlayer TargetPlayer)
{
	if (!PowerUpData)
	{
		return nullptr;
	}

	for (FActivePongPowerUp& ActivePowerUp : ActivePowerUps)
	{
		if (ActivePowerUp.Data && ActivePowerUp.Data->PowerUpId == PowerUpData->PowerUpId && ActivePowerUp.TargetPlayer == TargetPlayer)
		{
			return &ActivePowerUp;
		}
	}

	return nullptr;
}

bool UPongPowerUpComponent::StartPowerUpInstance(const UPongPowerUpDataAsset* PowerUpData, EPongPlayer TargetPlayer, AActor* SourceItem)
{
	APongGameMode* GameMode = GetPongGameMode();
	if (!GameMode)
	{
		return false;
	}

	UPongPowerUpEffect* Effect = NewObject<UPongPowerUpEffect>(this, PowerUpData->EffectClass);
	if (!Effect)
	{
		return false;
	}

	Effect->Initialize(PowerUpData);
	Effect->Apply(GameMode, TargetPlayer);

	FActivePongPowerUp& ActivePowerUp = ActivePowerUps.AddDefaulted_GetRef();
	ActivePowerUp.Data = const_cast<UPongPowerUpDataAsset*>(PowerUpData);
	ActivePowerUp.Effect = Effect;
	ActivePowerUp.TargetPlayer = TargetPlayer;
	ActivePowerUp.SourceItem = SourceItem;
	ActivePowerUp.StartTime = GetWorld()->GetTimeSeconds();
	ActivePowerUp.EndTime = ActivePowerUp.StartTime + FMath::Max(PowerUpData->Duration, 0.0f);

	if (PowerUpData->Duration > 0.0f)
	{
		FTimerDelegate ExpirationDelegate;
		ExpirationDelegate.BindUObject(this, &UPongPowerUpComponent::ExpirePowerUp, Effect);
		GetWorld()->GetTimerManager().SetTimer(ActivePowerUp.ExpirationTimerHandle, ExpirationDelegate, PowerUpData->Duration, false);
	}

	GameMode->NotifyPowerUpStarted(TargetPlayer, Effect, PowerUpData, PowerUpData->Duration);
	return true;
}

void UPongPowerUpComponent::RefreshPowerUp(FActivePongPowerUp& ActivePowerUp)
{
	if (!ActivePowerUp.Data || ActivePowerUp.Data->Duration <= 0.0f || !ActivePowerUp.Effect)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(ActivePowerUp.ExpirationTimerHandle);
	ActivePowerUp.StartTime = GetWorld()->GetTimeSeconds();
	ActivePowerUp.EndTime = ActivePowerUp.StartTime + ActivePowerUp.Data->Duration;

	FTimerDelegate ExpirationDelegate;
	ExpirationDelegate.BindUObject(this, &UPongPowerUpComponent::ExpirePowerUp, ActivePowerUp.Effect.Get());
	GetWorld()->GetTimerManager().SetTimer(ActivePowerUp.ExpirationTimerHandle, ExpirationDelegate, ActivePowerUp.Data->Duration, false);

	if (APongGameMode* GameMode = GetPongGameMode())
	{
		GameMode->NotifyPowerUpRefreshed(ActivePowerUp.Effect, ActivePowerUp.Data->Duration);
	}
}

void UPongPowerUpComponent::ExtendPowerUpDuration(FActivePongPowerUp& ActivePowerUp)
{
	if (!ActivePowerUp.Data || ActivePowerUp.Data->Duration <= 0.0f || !ActivePowerUp.Effect || !GetWorld())
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	ActivePowerUp.EndTime = FMath::Max(ActivePowerUp.EndTime, CurrentTime) + ActivePowerUp.Data->Duration;

	const float NewRemainingTime = FMath::Max(ActivePowerUp.EndTime - CurrentTime, 0.0f);
	GetWorld()->GetTimerManager().ClearTimer(ActivePowerUp.ExpirationTimerHandle);

	FTimerDelegate ExpirationDelegate;
	ExpirationDelegate.BindUObject(this, &UPongPowerUpComponent::ExpirePowerUp, ActivePowerUp.Effect.Get());
	GetWorld()->GetTimerManager().SetTimer(ActivePowerUp.ExpirationTimerHandle, ExpirationDelegate, NewRemainingTime, false);

	if (APongGameMode* GameMode = GetPongGameMode())
	{
		GameMode->NotifyPowerUpRefreshed(ActivePowerUp.Effect, NewRemainingTime);
	}
}

void UPongPowerUpComponent::ExpirePowerUp(UPongPowerUpEffect* Effect)
{
	for (int32 Index = ActivePowerUps.Num() - 1; Index >= 0; --Index)
	{
		if (ActivePowerUps[Index].Effect == Effect)
		{
			RemovePowerUpAtIndex(Index);
			return;
		}
	}
}

void UPongPowerUpComponent::RemovePowerUpAtIndex(int32 Index)
{
	if (!ActivePowerUps.IsValidIndex(Index))
	{
		return;
	}

	FActivePongPowerUp ActivePowerUp = ActivePowerUps[Index];
	GetWorld()->GetTimerManager().ClearTimer(ActivePowerUp.ExpirationTimerHandle);

	if (APongGameMode* GameMode = GetPongGameMode())
	{
		if (ActivePowerUp.Effect)
		{
			ActivePowerUp.Effect->Remove(GameMode, ActivePowerUp.TargetPlayer);
			GameMode->NotifyPowerUpRemoved(ActivePowerUp.Effect);
		}
	}

	ActivePowerUps.RemoveAtSwap(Index);
}
