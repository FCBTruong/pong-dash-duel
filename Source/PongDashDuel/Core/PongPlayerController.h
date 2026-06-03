// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PongTypes.h"
#include "GameFramework/PlayerController.h"
#include "PongPlayerController.generated.h"

class APongGameMode;
class APongPaddle;
class UInputAction;
class UInputMappingContext;
class UPongPowerUpDataAsset;
struct FInputActionValue;

UCLASS()
class PONGDASHDUEL_API APongPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	void MovePlayer1(const FInputActionValue& Value);
	void MovePlayer2(const FInputActionValue& Value);
	void StopPlayer1(const FInputActionValue& Value);
	void StopPlayer2(const FInputActionValue& Value);
	void Restart(const FInputActionValue& Value);
	void CheatGiveBulletPowerUp();

private:
	void CacheReferences();

	UPROPERTY()
	TObjectPtr<APongGameMode> PongGameMode;

	UPROPERTY()
	TObjectPtr<APongPaddle> Player1Paddle;

	UPROPERTY()
	TObjectPtr<APongPaddle> Player2Paddle;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MovePlayer1Action;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MovePlayer2Action;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> RestartAction;

	UPROPERTY(EditDefaultsOnly, Category = "Cheat")
	TObjectPtr<UPongPowerUpDataAsset> CheatBulletPowerUpData;

	UPROPERTY(EditDefaultsOnly, Category = "Cheat")
	EPongPlayer CheatBulletTargetPlayer = EPongPlayer::Player1;
};
