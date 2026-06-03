// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongPowerUpSpawner.h"

#include "PongPowerUpDataAsset.h"
#include "PongPowerUpItem.h"
#include "TimerManager.h"

APongPowerUpSpawner::APongPowerUpSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APongPowerUpSpawner::BeginPlay()
{
	Super::BeginPlay();
	BuildSpawnPoints();

	if (SpawnInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &APongPowerUpSpawner::SpawnPowerUp, SpawnInterval, true, SpawnInterval);
	}
}

void APongPowerUpSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void APongPowerUpSpawner::SpawnPowerUp()
{
	CleanupSpawnedItems();

	if (!ItemClass || GetActiveItemCount() >= MaxActiveItems)
	{
		return;
	}

	UPongPowerUpDataAsset* PowerUpData = GetRandomPowerUpData();
	if (!PowerUpData)
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;

	const int32 PointIndex = GetRandomAvailablePointIndex();
	if (!SpawnPoints.IsValidIndex(PointIndex))
	{
		return;
	}

	APongPowerUpItem* Item = GetWorld()->SpawnActor<APongPowerUpItem>(ItemClass, SpawnPoints[PointIndex], FRotator::ZeroRotator, SpawnParameters);
	if (!Item)
	{
		return;
	}

	Item->SetPowerUpData(PowerUpData);
	SpawnedItemsByPoint[PointIndex] = Item;
}

void APongPowerUpSpawner::BuildSpawnPoints()
{
	SpawnPoints.Reset();
	SpawnedItemsByPoint.Reset();

	const float SafeCellSize = FMath::Max(GridCellSize, 1.0f);
	const int32 CellCountX = FMath::Max(1, FMath::FloorToInt((SpawnRangeX.Y - SpawnRangeX.X) / SafeCellSize));
	const int32 CellCountY = FMath::Max(1, FMath::FloorToInt((SpawnRangeY.Y - SpawnRangeY.X) / SafeCellSize));

	const int32 CenterCellX = FMath::FloorToInt((0.0f - SpawnRangeX.X) / SafeCellSize);
	const int32 CenterCellY = FMath::FloorToInt((0.0f - SpawnRangeY.X) / SafeCellSize);

	for (int32 CellX = 0; CellX < CellCountX; ++CellX)
	{
		for (int32 CellY = 0; CellY < CellCountY; ++CellY)
		{
			if (bAvoidCenterCell && CellX == CenterCellX && CellY == CenterCellY)
			{
				continue;
			}

			SpawnPoints.Add(FVector(
				SpawnRangeX.X + SafeCellSize * (static_cast<float>(CellX) + 0.5f),
				SpawnRangeY.X + SafeCellSize * (static_cast<float>(CellY) + 0.5f),
				SpawnZ));
			SpawnedItemsByPoint.Add(nullptr);
		}
	}
}

void APongPowerUpSpawner::CleanupSpawnedItems()
{
	for (TWeakObjectPtr<APongPowerUpItem>& SpawnedItem : SpawnedItemsByPoint)
	{
		if (!SpawnedItem.IsValid())
		{
			SpawnedItem.Reset();
		}
	}
}

int32 APongPowerUpSpawner::GetRandomAvailablePointIndex() const
{
	TArray<int32> AvailablePointIndices;
	for (int32 Index = 0; Index < SpawnPoints.Num(); ++Index)
	{
		if (!SpawnedItemsByPoint.IsValidIndex(Index) || !SpawnedItemsByPoint[Index].IsValid())
		{
			AvailablePointIndices.Add(Index);
		}
	}

	if (AvailablePointIndices.IsEmpty())
	{
		return INDEX_NONE;
	}

	return AvailablePointIndices[FMath::RandRange(0, AvailablePointIndices.Num() - 1)];
}

int32 APongPowerUpSpawner::GetActiveItemCount() const
{
	int32 ActiveItemCount = 0;
	for (const TWeakObjectPtr<APongPowerUpItem>& SpawnedItem : SpawnedItemsByPoint)
	{
		if (SpawnedItem.IsValid())
		{
			++ActiveItemCount;
		}
	}

	return ActiveItemCount;
}

UPongPowerUpDataAsset* APongPowerUpSpawner::GetRandomPowerUpData() const
{
	if (PowerUpDataList.IsEmpty())
	{
		return nullptr;
	}

	return PowerUpDataList[FMath::RandRange(0, PowerUpDataList.Num() - 1)];
}
