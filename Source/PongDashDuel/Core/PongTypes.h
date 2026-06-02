// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PongTypes.generated.h"

UENUM(BlueprintType)
enum class EPongPlayer : uint8
{
	None,
	Player1,
	Player2
};

UENUM(BlueprintType)
enum class EPongMatchState : uint8
{
	WaitingToStart,
	Playing,
	MatchEnded
};
