// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PongPowerUpSpawner.generated.h"

class APongPowerUpItem;
class UPongPowerUpDataAsset;

UCLASS()
class PONGDASHDUEL_API APongPowerUpSpawner : public AActor
{
	GENERATED_BODY()

public:
	APongPowerUpSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SpawnPowerUp();
	void BuildSpawnPoints();
	void CleanupSpawnedItems();
	int32 GetRandomAvailablePointIndex() const;
	int32 GetActiveItemCount() const;
	UPongPowerUpDataAsset* GetRandomPowerUpData() const;

	UPROPERTY(EditAnywhere, Category = "PowerUp")
	TSubclassOf<APongPowerUpItem> ItemClass;

	UPROPERTY(EditAnywhere, Category = "PowerUp")
	TArray<TObjectPtr<UPongPowerUpDataAsset>> PowerUpDataList;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float SpawnInterval = 8.0f;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 MaxActiveItems = 2;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	FVector2D SpawnRangeX = FVector2D(-280.0f, 280.0f);

	UPROPERTY(EditAnywhere, Category = "Spawn")
	FVector2D SpawnRangeY = FVector2D(-250.0f, 250.0f);

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float SpawnZ = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float GridCellSize = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	bool bAvoidCenterCell = true;

private:
	FTimerHandle SpawnTimerHandle;

	UPROPERTY()
	TArray<FVector> SpawnPoints;

	UPROPERTY()
	TArray<TWeakObjectPtr<APongPowerUpItem>> SpawnedItemsByPoint;
};
