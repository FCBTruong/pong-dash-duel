// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PongGameDataAsset.generated.h"

class UNiagaraSystem;
class UParticleSystem;
class USoundBase;

UCLASS(BlueprintType)
class PONGDASHDUEL_API UPongGameDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> BounceSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX")
	TObjectPtr<UParticleSystem> BounceImpactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Goal")
	TObjectPtr<UNiagaraSystem> GoalExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Goal")
	TObjectPtr<USoundBase> GoalSound;
};
