// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongBall.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Core/PongCollisionChannels.h"
#include "../Core/PongGameDataAsset.h"
#include "../Core/PongGameMode.h"
#include "PongPaddle.h"
#include "../PowerUps/WorldObjects/PongPowerProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "UObject/ConstructorHelpers.h"

APongBall::APongBall()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetSphereRadius(18.0f);
	ConfigureCollision();

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetRelativeScale3D(FVector(0.36f));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(SphereMesh.Object);
	}
}

void APongBall::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void APongBall::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();
	CurrentSpeed = InitialSpeed;

	UE_LOG(LogTemp, Log, TEXT("PongBall BeginPlay: %s Location=%s Radius=%f"),
		*GetName(),
		*GetActorLocation().ToString(),
		CollisionComponent->GetScaledSphereRadius());
}

void APongBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Move(DeltaTime);
}

void APongBall::Launch(float DirectionX)
{
	const float YSign = DirectionX >= 0.0f ? 1.0f : -1.0f;
	CurrentSpeed = InitialSpeed;
	Velocity = FVector(FMath::RandBool() ? 0.35f : -0.35f, YSign, 0.0f).GetSafeNormal() * CurrentSpeed * SpeedMultiplier * DangerAreaSpeedMultiplier;
}

void APongBall::Stop()
{
	Velocity = FVector::ZeroVector;
}

void APongBall::ResetBall()
{
	SetActorLocation(InitialLocation, false, nullptr, ETeleportType::TeleportPhysics);
	ShowBall();
	ResetTrail();
	Stop();
	CurrentSpeed = InitialSpeed;
	DangerAreaSpeedMultiplier = 1.0f;
	LastHitPlayer = EPongPlayer::None;
}

void APongBall::ResetBallHidden()
{
	SetActorLocation(InitialLocation, false, nullptr, ETeleportType::TeleportPhysics);
	HideBall();
	ResetTrail();
	CurrentSpeed = InitialSpeed;
	DangerAreaSpeedMultiplier = 1.0f;
	LastHitPlayer = EPongPlayer::None;
}

void APongBall::HideBall()
{
	Stop();
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void APongBall::ShowBall()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	ConfigureCollision();
	ResetTrail();
}

void APongBall::PlaySpawnFeedback() const
{
	if (const APongGameMode* GameMode = Cast<APongGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (const UPongGameDataAsset* GameData = GameMode->GetGameData())
		{
			if (GameData->BallSpawnEffect)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, GameData->BallSpawnEffect, GetActorLocation());
			}
		}
	}
}

void APongBall::BounceFromWall(const FVector& ImpactLocation, const FVector& ImpactNormal)
{
	const FVector SafeNormal = ImpactNormal.GetSafeNormal();
	Velocity = Velocity - 2.0f * FVector::DotProduct(Velocity, SafeNormal) * SafeNormal;
	Velocity = Velocity.GetSafeNormal() * CurrentSpeed * SpeedMultiplier * DangerAreaSpeedMultiplier;

	const float BallRadius = CollisionComponent->GetScaledSphereRadius();
	SetActorLocation(ImpactLocation + SafeNormal * (BallRadius + 2.0f), false, nullptr, ETeleportType::TeleportPhysics);

	PlayBounceFeedback(ImpactLocation, ImpactNormal);
}

void APongBall::PlayBounceFeedback(const FVector& Location, const FVector& Normal)
{
	if (const APongGameMode* GameMode = Cast<APongGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (const UPongGameDataAsset* GameData = GameMode->GetGameData())
		{
			if (GameData->BounceImpactEffect)
			{
				const FRotator EffectRotation = Normal.GetSafeNormal().Rotation();
				UGameplayStatics::SpawnEmitterAtLocation(this, GameData->BounceImpactEffect, Location, EffectRotation);
			}

			if (GameData->BounceSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, GameData->BounceSound, Location);
			}
		}
	}
}

void APongBall::BounceFromPaddle(const APongPaddle* Paddle, const FVector& ImpactLocation, const FVector& ImpactNormal)
{
	if (!Paddle)
	{
		return;
	}

	LastHitPlayer = Paddle->GetOwningPlayer();
	const float PreviousYSign = Velocity.Y >= 0.0f ? 1.0f : -1.0f;
	const float HitOffset = FMath::Clamp((GetActorLocation().X - Paddle->GetActorLocation().X) / Paddle->GetHalfHeight(), -1.0f, 1.0f);
	CurrentSpeed = FMath::Min(CurrentSpeed + SpeedIncreaseOnPaddleHit, MaxSpeed);

	const float YSign = PreviousYSign >= 0.0f ? -1.0f : 1.0f;
	Velocity.Y = YSign * FMath::Max(FMath::Abs(Velocity.Y), 1.0f);
	Velocity.X = HitOffset * BounceXStrength;
	Velocity = Velocity.GetSafeNormal() * CurrentSpeed * SpeedMultiplier * Paddle->GetBallHitSpeedMultiplier() * DangerAreaSpeedMultiplier;

	const float PaddleHalfWidth = 18.0f;
	const float BallRadius = CollisionComponent->GetScaledSphereRadius();
	FVector SafeLocation = GetActorLocation();
	SafeLocation.Y = Paddle->GetActorLocation().Y - PreviousYSign * (PaddleHalfWidth + BallRadius + 2.0f);
	SetActorLocation(SafeLocation, false, nullptr, ETeleportType::TeleportPhysics);
	PlayBounceFeedback(ImpactLocation, ImpactNormal);
}

void APongBall::Move(float DeltaTime)
{
	if (Velocity.IsNearlyZero())
	{
		return;
	}

	FHitResult Hit;
	SetActorLocation(GetActorLocation() + Velocity * DeltaTime, true, &Hit);
	CollisionComponent->UpdateOverlaps();

	if (Hit.bBlockingHit)
	{
		if (APongPowerProjectile* Projectile = Cast<APongPowerProjectile>(Hit.GetActor()))
		{
			Projectile->Destroy();
			return;
		}

		if (const APongPaddle* Paddle = Cast<APongPaddle>(Hit.GetActor()))
		{
			BounceFromPaddle(Paddle, Hit.ImpactPoint, Hit.ImpactNormal);
		}
		else
		{
			BounceFromWall(Hit.ImpactPoint, Hit.ImpactNormal);
		}
	}

}

void APongBall::ConfigureCollision()
{
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionObjectType(COLLISION_BALL);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(COLLISION_PADDLE, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(COLLISION_WALL, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(COLLISION_GOALZONE, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(COLLISION_POWERUP, ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);
}

void APongBall::SetSpeedMultiplier(float NewSpeedMultiplier)
{
	SpeedMultiplier = FMath::Max(NewSpeedMultiplier, 0.1f);

	if (!Velocity.IsNearlyZero())
	{
		Velocity = Velocity.GetSafeNormal() * CurrentSpeed * SpeedMultiplier * DangerAreaSpeedMultiplier;
	}
}

void APongBall::ApplyPaddleHitSpeedMultiplier(float HitSpeedMultiplier)
{
	if (Velocity.IsNearlyZero())
	{
		return;
	}

	const float SafeHitSpeedMultiplier = FMath::Max(HitSpeedMultiplier, 0.1f);
	Velocity = Velocity.GetSafeNormal() * CurrentSpeed * SpeedMultiplier * SafeHitSpeedMultiplier * DangerAreaSpeedMultiplier;
}

void APongBall::EnterDangerArea(float SlowMultiplier)
{
	DangerAreaSpeedMultiplier = FMath::Clamp(SlowMultiplier, 0.1f, 1.0f);

	if (!Velocity.IsNearlyZero())
	{
		Velocity = Velocity.GetSafeNormal() * CurrentSpeed * SpeedMultiplier * DangerAreaSpeedMultiplier;
	}
}

void APongBall::ExitDangerArea(float ExitSpeedMultiplier, float RandomAngleDegrees)
{
	if (Velocity.IsNearlyZero())
	{
		DangerAreaSpeedMultiplier = 1.0f;
		return;
	}

	const float RandomAngle = FMath::FRandRange(-RandomAngleDegrees, RandomAngleDegrees);
	const FVector RotatedDirection = Velocity.GetSafeNormal().RotateAngleAxis(RandomAngle, FVector::UpVector);

	DangerAreaSpeedMultiplier = 1.0f;
	const float ClampedExitMultiplier = FMath::Max(ExitSpeedMultiplier, 1.0f);
	Velocity = RotatedDirection.GetSafeNormal() * CurrentSpeed * SpeedMultiplier * ClampedExitMultiplier;
}

void APongBall::ResetTrail()
{
	TArray<UNiagaraComponent*> NiagaraComponents;
	GetComponents<UNiagaraComponent>(NiagaraComponents);

	for (UNiagaraComponent* NiagaraComponent : NiagaraComponents)
	{
		if (!NiagaraComponent || !NiagaraComponent->GetName().Contains(TEXT("Trail")))
		{
			continue;
		}

		NiagaraComponent->DeactivateImmediate();
		NiagaraComponent->ResetSystem();
		NiagaraComponent->Activate(true);
	}
}
