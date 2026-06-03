// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PongPowerUpTypes.generated.h"

UENUM(BlueprintType)
enum class EPongPowerUpStackPolicy : uint8
{
	IgnoreIfActive,
	RefreshDuration,
	Replace,
	Stack
};
