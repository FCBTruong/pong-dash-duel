// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/PongTypes.h"
#include "GameFramework/Actor.h"
#include "PongDefenseShield.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class PONGDASHDUEL_API APongDefenseShield : public AActor
{
	GENERATED_BODY()

public:
	APongDefenseShield();

	void ActivateShield();
	void DeactivateShield();
	bool IsShieldActive() const { return bIsActive; }
	EPongPlayer GetOwningPlayer() const { return OwningPlayer; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shield")
	EPongPlayer OwningPlayer = EPongPlayer::None;

private:
	bool bIsActive = false;
};
