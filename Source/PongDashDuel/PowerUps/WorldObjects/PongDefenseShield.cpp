// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongDefenseShield.h"

#include "../../Core/PongCollisionChannels.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

APongDefenseShield::APongDefenseShield()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetBoxExtent(FVector(260.0f, 18.0f, 60.0f));
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->SetCollisionObjectType(COLLISION_WALL);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(COLLISION_BALL, ECR_Block);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetRelativeScale3D(FVector(5.2f, 0.36f, 1.2f));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CubeMesh.Object);
	}
}

void APongDefenseShield::BeginPlay()
{
	Super::BeginPlay();
	DeactivateShield();
}

void APongDefenseShield::ActivateShield()
{
	bIsActive = true;
	SetActorHiddenInGame(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void APongDefenseShield::DeactivateShield()
{
	bIsActive = false;
	SetActorHiddenInGame(true);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
