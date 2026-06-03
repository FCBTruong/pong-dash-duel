// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../PongPowerUpEffect.h"
#include "PongShootProjectilePowerUpEffect.generated.h"

class APongPowerProjectile;
class UNiagaraSystem;

UCLASS()
class PONGDASHDUEL_API UPongShootProjectilePowerUpEffect : public UPongPowerUpEffect
{
	GENERATED_BODY()

public:
	virtual void Apply(APongGameMode* GameMode, EPongPlayer TargetPlayer) override;
	virtual void Remove(APongGameMode* GameMode, EPongPlayer TargetPlayer) override;

protected:
	void FireProjectile();

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<APongPowerProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float FireInterval = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float SpawnForwardOffset = 70.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|VFX")
	TObjectPtr<UNiagaraSystem> NiagaraShoot;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|VFX")
	TObjectPtr<UNiagaraSystem> NiagaraExplode;

private:
	UPROPERTY()
	TObjectPtr<APongGameMode> CachedGameMode;

	EPongPlayer CachedTargetPlayer = EPongPlayer::None;
	FTimerHandle FireTimerHandle;
};
