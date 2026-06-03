// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PongPowerUpTypes.h"
#include "PongPowerUpDataAsset.generated.h"

class UNiagaraSystem;
class UPongPowerUpEffect;
class USoundBase;
class UTexture2D;

UCLASS(BlueprintType)
class PONGDASHDUEL_API UPongPowerUpDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerUp")
	FName PowerUpId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerUp")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerUp")
	TSubclassOf<UPongPowerUpEffect> EffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerUp")
	float Duration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerUp")
	EPongPowerUpStackPolicy StackPolicy = EPongPowerUpStackPolicy::RefreshDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UNiagaraSystem> PickupEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<USoundBase> PickupSound;
};
