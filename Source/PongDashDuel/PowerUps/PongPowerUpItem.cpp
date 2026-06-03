// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongPowerUpItem.h"

#include "../Core/PongCollisionChannels.h"
#include "../Core/PongGameMode.h"
#include "../Gameplay/PongBall.h"
#include "Components/BillboardComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "PongPowerUpComponent.h"
#include "PongPowerUpDataAsset.h"
#include "UObject/ConstructorHelpers.h"

APongPowerUpItem::APongPowerUpItem()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetSphereRadius(55.0f);
	ConfigureCollision();

	VisualRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
	VisualRootComponent->SetupAttachment(CollisionComponent);

	IconComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Icon"));
	IconComponent->SetupAttachment(VisualRootComponent);
	IconComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IconComponent->SetHiddenInGame(true);
	IconComponent->SetVisibility(false);
	IconComponent->bIsScreenSizeScaled = false;

	DebugCollisionMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugCollisionMesh"));
	DebugCollisionMeshComponent->SetupAttachment(CollisionComponent);
	DebugCollisionMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DebugCollisionMeshComponent->SetHiddenInGame(true);
	DebugCollisionMeshComponent->SetVisibility(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		DebugCollisionMeshComponent->SetStaticMesh(SphereMesh.Object);
	}

}

void APongPowerUpItem::SetPowerUpData(UPongPowerUpDataAsset* InPowerUpData)
{
	PowerUpData = InPowerUpData;
	ApplyVisualsFromData();
}

void APongPowerUpItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ConfigureCollision();
	ApplyVisualsFromData();
	UpdateDebugCollisionVisual();
	UpdateIconWorldScale();
}

void APongPowerUpItem::BeginPlay()
{
	Super::BeginPlay();
	ConfigureCollision();
	ApplyVisualsFromData();
	UpdateDebugCollisionVisual();
	UpdateIconWorldScale();
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APongPowerUpItem::OnPowerUpOverlap);
}

void APongPowerUpItem::ConfigureCollision()
{
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(COLLISION_POWERUP);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(COLLISION_BALL, ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);
}

void APongPowerUpItem::UpdateDebugCollisionVisual()
{
	if (!DebugCollisionMeshComponent)
	{
		return;
	}

	const float Radius = CollisionComponent->GetUnscaledSphereRadius();
	DebugCollisionMeshComponent->SetRelativeScale3D(FVector(Radius / 50.0f));
	DebugCollisionMeshComponent->SetHiddenInGame(!bShowDebugCollisionInGame);
	DebugCollisionMeshComponent->SetVisibility(bShowDebugCollisionInGame);
}

void APongPowerUpItem::UpdateIconWorldScale()
{
	if (!IconComponent)
	{
		return;
	}

	constexpr float IconFitRatio = 0.72f;
	const float Radius = CollisionComponent->GetUnscaledSphereRadius();
	const float IconScale = (Radius * IconFitRatio) / 128.0f;
	IconComponent->SetRelativeScale3D(FVector(IconScale));
}

void APongPowerUpItem::ApplyVisualsFromData()
{
	if (!PowerUpData)
	{
		return;
	}

	if (PowerUpData->IconTexture)
	{
		IconComponent->SetSprite(PowerUpData->IconTexture);
		UpdateIconWorldScale();
		IconComponent->SetHiddenInGame(false);
		IconComponent->SetVisibility(true);
	}
}

void APongPowerUpItem::PlayPickupFeedback() const
{
	if (!PowerUpData)
	{
		return;
	}

	if (PowerUpData->PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PowerUpData->PickupEffect, GetActorLocation());
	}

	if (PowerUpData->PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PowerUpData->PickupSound, GetActorLocation());
	}
}

void APongPowerUpItem::OnPowerUpOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!PowerUpData)
	{
		return;
	}

	const APongBall* Ball = Cast<APongBall>(OtherActor);
	if (!Ball || Ball->GetLastHitPlayer() == EPongPlayer::None)
	{
		return;
	}

	APongGameMode* GameMode = Cast<APongGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode || !GameMode->GetPowerUpComponent())
	{
		return;
	}

	if (GameMode->GetPowerUpComponent()->ApplyPowerUp(PowerUpData, Ball->GetLastHitPlayer(), this))
	{
		PlayPickupFeedback();
		Destroy();
	}
}
