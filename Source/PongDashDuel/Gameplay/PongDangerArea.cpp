// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongDangerArea.h"

#include "../Core/PongCollisionChannels.h"
#include "Components/SphereComponent.h"
#include "PongBall.h"

APongDangerArea::APongDangerArea()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComponent);
	ConfigureCollision();
}

void APongDangerArea::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ConfigureCollision();
}

void APongDangerArea::BeginPlay()
{
	Super::BeginPlay();
	ConfigureCollision();
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APongDangerArea::OnDangerBeginOverlap);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &APongDangerArea::OnDangerEndOverlap);
}

void APongDangerArea::ConfigureCollision()
{
	CollisionComponent->SetSphereRadius(AreaRadius);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(COLLISION_BALL, ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);
}

void APongDangerArea::OnDangerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APongBall* Ball = Cast<APongBall>(OtherActor))
	{
		Ball->EnterDangerArea(SlowMultiplier);
	}
}

void APongDangerArea::OnDangerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (APongBall* Ball = Cast<APongBall>(OtherActor))
	{
		Ball->ExitDangerArea(ExitSpeedMultiplier, ExitRandomAngleDegrees);
	}
}
