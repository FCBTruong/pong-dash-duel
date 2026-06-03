// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongShootProjectilePowerUpEffect.h"

#include "../../Core/PongGameMode.h"
#include "../../Gameplay/PongPaddle.h"
#include "../WorldObjects/PongPowerProjectile.h"
#include "TimerManager.h"

void UPongShootProjectilePowerUpEffect::Apply(APongGameMode* GameMode, EPongPlayer TargetPlayer)
{
	if (!GameMode || TargetPlayer == EPongPlayer::None)
	{
		return;
	}

	CachedGameMode = GameMode;
	CachedTargetPlayer = TargetPlayer;

	FireProjectile();

	if (UWorld* World = GameMode->GetWorld())
	{
		World->GetTimerManager().SetTimer(FireTimerHandle, this, &UPongShootProjectilePowerUpEffect::FireProjectile, FireInterval, true, FireInterval);
	}
}

void UPongShootProjectilePowerUpEffect::Remove(APongGameMode* GameMode, EPongPlayer TargetPlayer)
{
	if (GameMode)
	{
		if (UWorld* World = GameMode->GetWorld())
		{
			World->GetTimerManager().ClearTimer(FireTimerHandle);
		}
	}

	CachedGameMode = nullptr;
	CachedTargetPlayer = EPongPlayer::None;
}

void UPongShootProjectilePowerUpEffect::FireProjectile()
{
	if (!CachedGameMode || CachedTargetPlayer == EPongPlayer::None)
	{
		return;
	}

	APongPaddle* Paddle = CachedGameMode->GetPaddle(CachedTargetPlayer);
	if (!Paddle)
	{
		return;
	}

	UWorld* World = CachedGameMode->GetWorld();
	if (!World)
	{
		return;
	}

	const FVector FireDirection = CachedTargetPlayer == EPongPlayer::Player1 ? FVector(0.0f, 1.0f, 0.0f) : FVector(0.0f, -1.0f, 0.0f);
	const FVector SpawnLocation = Paddle->GetActorLocation() + FireDirection * SpawnForwardOffset;
	const FRotator SpawnRotation = FireDirection.Rotation();

	UClass* ClassToSpawn = ProjectileClass ? ProjectileClass.Get() : APongPowerProjectile::StaticClass();
	APongPowerProjectile* Projectile = World->SpawnActor<APongPowerProjectile>(ClassToSpawn, SpawnLocation, SpawnRotation);
	if (Projectile)
	{
		Projectile->InitializeProjectile(CachedTargetPlayer, FireDirection, NiagaraShoot, NiagaraExplode);
	}
}
