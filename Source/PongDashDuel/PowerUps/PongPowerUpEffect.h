// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Core/PongTypes.h"
#include "UObject/Object.h"
#include "PongPowerUpEffect.generated.h"

class APongGameMode;
class UPongPowerUpDataAsset;

UCLASS(Blueprintable, Abstract)
class PONGDASHDUEL_API UPongPowerUpEffect : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(const UPongPowerUpDataAsset* InData);
	virtual void Apply(APongGameMode* GameMode, EPongPlayer TargetPlayer);
	virtual void Remove(APongGameMode* GameMode, EPongPlayer TargetPlayer);

	const UPongPowerUpDataAsset* GetData() const { return Data; }

protected:
	UPROPERTY()
	TObjectPtr<UPongPowerUpDataAsset> Data;
};
