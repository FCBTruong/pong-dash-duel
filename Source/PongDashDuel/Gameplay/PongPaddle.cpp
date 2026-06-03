// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongPaddle.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Core/PongCollisionChannels.h"
#include "UObject/ConstructorHelpers.h"

APongPaddle::APongPaddle()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetBoxExtent(FVector(HalfHeight, HalfWidth, HalfDepth));
	ConfigureCollision();

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetRelativeScale3D(FVector(HalfHeight, HalfWidth, HalfDepth) / 50.0f);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CubeMesh.Object);
	}
}

void APongPaddle::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetActorRotation(FRotator::ZeroRotator);
	CollisionComponent->SetBoxExtent(FVector(HalfHeight, HalfWidth, HalfDepth));
	ConfigureCollision();
	MeshComponent->SetRelativeLocation(FVector::ZeroVector);
	MeshComponent->SetRelativeRotation(FRotator::ZeroRotator);
	MeshComponent->SetRelativeScale3D(FVector(HalfHeight, HalfWidth, HalfDepth) / 50.0f);
}

void APongPaddle::BeginPlay()
{
	Super::BeginPlay();
	ConfigureCollision();
	InitialLocation = GetActorLocation();
	BaseHalfHeight = HalfHeight;
	ClampPosition();
}

void APongPaddle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Move(DeltaTime);
}

void APongPaddle::SetMoveInput(float Value)
{
	MoveInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void APongPaddle::ResetPaddle()
{
	SetActorLocation(InitialLocation);
	MoveInput = 0.0f;
}

float APongPaddle::GetHalfHeight() const
{
	return HalfHeight;
}

void APongPaddle::SetPaddleSizeMultiplier(float NewSizeMultiplier)
{
	const float ClampedMultiplier = FMath::Max(NewSizeMultiplier, 0.1f);
	HalfHeight = BaseHalfHeight * ClampedMultiplier;
	CollisionComponent->SetBoxExtent(FVector(HalfHeight, HalfWidth, HalfDepth));
	MeshComponent->SetRelativeScale3D(FVector(HalfHeight, HalfWidth, HalfDepth) / 50.0f);
}

void APongPaddle::SetBallHitSpeedMultiplier(float NewSpeedMultiplier)
{
	BallHitSpeedMultiplier = FMath::Max(NewSpeedMultiplier, 0.1f);
}

void APongPaddle::Move(float DeltaTime)
{
	if (FMath::IsNearlyZero(MoveInput))
	{
		return;
	}

	const FVector Delta = FVector(MoveInput * MoveSpeed * DeltaTime, 0.0f, 0.0f);
	SetActorLocation(GetActorLocation() + Delta);
	ClampPosition();
}

void APongPaddle::ClampPosition()
{
	FVector Location = GetActorLocation();
	Location.X = FMath::Clamp(Location.X, MinX, MaxX);
	SetActorLocation(Location);
}

void APongPaddle::ConfigureCollision()
{
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionObjectType(COLLISION_PADDLE);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(COLLISION_BALL, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(COLLISION_GOALZONE, ECR_Ignore);
	CollisionComponent->SetGenerateOverlapEvents(false);
}
