// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/PongTypes.h"
#include "GameFramework/Actor.h"
#include "PongPowerProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraSystem;
class UPrimitiveComponent;

UCLASS()
class PONGDASHDUEL_API APongPowerProjectile : public AActor
{
	GENERATED_BODY()

public:
	APongPowerProjectile();

	virtual void Tick(float DeltaTime) override;

	void InitializeProjectile(EPongPlayer InOwnerPlayer, const FVector& InDirection, UNiagaraSystem* InNiagaraShoot = nullptr, UNiagaraSystem* InNiagaraExplode = nullptr);
	EPongPlayer GetOwnerPlayer() const { return OwnerPlayer; }
	void DestroyWithImpact(const FVector& ImpactLocation);

protected:
	virtual void BeginPlay() override;

	void ConfigureCollision();

	UFUNCTION()
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float MoveSpeed = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float LifeTime = 6.0f;

private:
	EPongPlayer OwnerPlayer = EPongPlayer::None;
	FVector MoveDirection = FVector::ZeroVector;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> NiagaraShoot;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> NiagaraExplode;

	bool bImpactDestroyStarted = false;
};
