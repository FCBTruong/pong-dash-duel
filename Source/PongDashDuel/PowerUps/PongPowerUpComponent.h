// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Core/PongTypes.h"
#include "Components/ActorComponent.h"
#include "PongPowerUpComponent.generated.h"

class APongGameMode;
class UPongPowerUpDataAsset;
class UPongPowerUpEffect;

USTRUCT()
struct FActivePongPowerUp
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UPongPowerUpDataAsset> Data;

	UPROPERTY()
	TObjectPtr<UPongPowerUpEffect> Effect;

	UPROPERTY()
	EPongPlayer TargetPlayer = EPongPlayer::None;

	UPROPERTY()
	TObjectPtr<AActor> SourceItem;

	UPROPERTY()
	float StartTime = 0.0f;

	UPROPERTY()
	float EndTime = 0.0f;

	FTimerHandle ExpirationTimerHandle;
};

UCLASS(ClassGroup = (Pong), meta = (BlueprintSpawnableComponent))
class PONGDASHDUEL_API UPongPowerUpComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPongPowerUpComponent();

	bool ApplyPowerUp(const UPongPowerUpDataAsset* PowerUpData, EPongPlayer TargetPlayer, AActor* SourceItem);
	void ClearAllPowerUps();

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	APongGameMode* GetPongGameMode() const;
	FActivePongPowerUp* FindActivePowerUp(const UPongPowerUpDataAsset* PowerUpData, EPongPlayer TargetPlayer);
	bool StartPowerUpInstance(const UPongPowerUpDataAsset* PowerUpData, EPongPlayer TargetPlayer, AActor* SourceItem);
	void RefreshPowerUp(FActivePongPowerUp& ActivePowerUp);
	void ExtendPowerUpDuration(FActivePongPowerUp& ActivePowerUp);
	void ExpirePowerUp(UPongPowerUpEffect* Effect);
	void RemovePowerUpAtIndex(int32 Index);

	UPROPERTY()
	TArray<FActivePongPowerUp> ActivePowerUps;
};
