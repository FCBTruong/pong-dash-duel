// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongGoalZone.h"

#include "Components/BoxComponent.h"
#include "../Core/PongCollisionChannels.h"
#include "../Core/PongGameMode.h"
#include "PongBall.h"
#include "../PowerUps/WorldObjects/PongPowerProjectile.h"
#include "Kismet/GameplayStatics.h"

APongGoalZone::APongGoalZone()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetBoxExtent(FVector(620.0f, 24.0f, 80.0f));
	ConfigureCollision();
}

void APongGoalZone::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ConfigureCollision();
}

void APongGoalZone::BeginPlay()
{
	Super::BeginPlay();
	ConfigureCollision();
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APongGoalZone::OnGoalOverlap);

	UE_LOG(LogTemp, Log, TEXT("PongGoalZone BeginPlay: %s ScoringPlayer=%d Location=%s Extent=%s"),
		*GetName(),
		static_cast<int32>(ScoringPlayer),
		*GetActorLocation().ToString(),
		*CollisionComponent->GetScaledBoxExtent().ToString());
}

void APongGoalZone::OnGoalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<APongBall>(OtherActor))
	{
	}
	else if (APongPowerProjectile* Projectile = Cast<APongPowerProjectile>(OtherActor))
	{
		Projectile->DestroyWithImpact(Projectile->GetActorLocation());
	}
	else
	{
		return;
	}

	if (APongGameMode* GameMode = Cast<APongGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->ScorePoint(ScoringPlayer);
	}
}

void APongGoalZone::ConfigureCollision()
{
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(COLLISION_GOALZONE);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(COLLISION_BALL, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(COLLISION_POWERUP, ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);
}
