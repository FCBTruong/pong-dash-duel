// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PongPowerUpItem.generated.h"

class UPongPowerUpDataAsset;
class UPrimitiveComponent;
class UBillboardComponent;
class USceneComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class PONGDASHDUEL_API APongPowerUpItem : public AActor
{
	GENERATED_BODY()

public:
	APongPowerUpItem();

	void SetPowerUpData(UPongPowerUpDataAsset* InPowerUpData);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	void ConfigureCollision();
	void ApplyVisualsFromData();
	void PlayPickupFeedback() const;
	void UpdateDebugCollisionVisual();
	void UpdateIconWorldScale();

	UFUNCTION()
	void OnPowerUpOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> VisualRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBillboardComponent> IconComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DebugCollisionMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerUp")
	TObjectPtr<UPongPowerUpDataAsset> PowerUpData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bShowDebugCollisionInGame = false;
};
