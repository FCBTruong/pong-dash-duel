// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongPowerProjectile.h"

#include "../../Core/PongCollisionChannels.h"
#include "../../Gameplay/PongBall.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "UObject/ConstructorHelpers.h"

APongPowerProjectile::APongPowerProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetSphereRadius(16.0f);
	ConfigureCollision();

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetRelativeScale3D(FVector(0.28f));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(SphereMesh.Object);
	}

	InitialLifeSpan = LifeTime;
}

void APongPowerProjectile::BeginPlay()
{
	Super::BeginPlay();
	ConfigureCollision();
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APongPowerProjectile::OnProjectileOverlap);
	SetLifeSpan(LifeTime);
}

void APongPowerProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveDirection.IsNearlyZero())
	{
		return;
	}

	FHitResult Hit;
	SetActorLocation(GetActorLocation() + MoveDirection * MoveSpeed * DeltaTime, true, &Hit);

	if (Hit.bBlockingHit)
	{
		DestroyWithImpact(Hit.ImpactPoint);
	}
}

void APongPowerProjectile::InitializeProjectile(EPongPlayer InOwnerPlayer, const FVector& InDirection, UNiagaraSystem* InNiagaraShoot, UNiagaraSystem* InNiagaraExplode)
{
	OwnerPlayer = InOwnerPlayer;
	MoveDirection = InDirection.GetSafeNormal();
	NiagaraShoot = InNiagaraShoot;
	NiagaraExplode = InNiagaraExplode;

	if (NiagaraShoot)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraShoot, GetActorLocation(), GetActorRotation());
	}
}

void APongPowerProjectile::DestroyWithImpact(const FVector& ImpactLocation)
{
	if (bImpactDestroyStarted)
	{
		return;
	}

	bImpactDestroyStarted = true;

	if (NiagaraExplode)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraExplode, ImpactLocation, GetActorRotation());
	}

	Destroy();
}

void APongPowerProjectile::ConfigureCollision()
{
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionObjectType(COLLISION_POWERUP);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(COLLISION_BALL, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(COLLISION_GOALZONE, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(COLLISION_POWERUP, ECR_Ignore);
	CollisionComponent->SetGenerateOverlapEvents(true);
}

void APongPowerProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bImpactDestroyStarted || !OtherActor || OtherActor == this)
	{
		return;
	}

	if (Cast<APongBall>(OtherActor))
	{
		Destroy();
	}
}
