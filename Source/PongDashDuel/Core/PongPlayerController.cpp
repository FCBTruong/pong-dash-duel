// Copyright Epic Games, Inc. All Rights Reserved.

#include "PongPlayerController.h"

#include "PongGameMode.h"
#include "PongTypes.h"
#include "../Gameplay/PongPaddle.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"

void APongPlayerController::BeginPlay()
{
	Super::BeginPlay();
	CacheReferences();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				InputSubsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

void APongPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PongGameMode || !Player1Paddle || !Player2Paddle)
	{
		CacheReferences();
	}

	ClearStaleMoveInput();
}

void APongPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("XXXGGSetting up input bindings in PongPlayerController"));
	if (MovePlayer1Action)
	{
		UE_LOG(LogTemp, Log, TEXT("XXXGGSetting Binding MovePlayer1Action"));
		EnhancedInputComponent->BindAction(MovePlayer1Action, ETriggerEvent::Triggered, this, &APongPlayerController::MovePlayer1);
	}

	if (MovePlayer2Action)
	{
		EnhancedInputComponent->BindAction(MovePlayer2Action, ETriggerEvent::Triggered, this, &APongPlayerController::MovePlayer2);
	}

	if (RestartAction)
	{
		EnhancedInputComponent->BindAction(RestartAction, ETriggerEvent::Started, this, &APongPlayerController::Restart);
	}
}

void APongPlayerController::MovePlayer1(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("MovePlayer1: %f"), Value.Get<float>());
	LastPlayer1MoveFrame = GFrameCounter;
	if (PongGameMode)
	{
		PongGameMode->NotifyPlayerMoveInput(Value.Get<float>());
	}
	if (Player1Paddle)
	{
		Player1Paddle->SetMoveInput(Value.Get<float>());
	}
}

void APongPlayerController::MovePlayer2(const FInputActionValue& Value)
{
	LastPlayer2MoveFrame = GFrameCounter;
	if (PongGameMode)
	{
		PongGameMode->NotifyPlayerMoveInput(Value.Get<float>());
	}
	if (Player2Paddle)
	{
		Player2Paddle->SetMoveInput(Value.Get<float>());
	}
}

void APongPlayerController::ClearStaleMoveInput()
{
	if (Player1Paddle && LastPlayer1MoveFrame != GFrameCounter)
	{
		Player1Paddle->SetMoveInput(0.0f);
	}

	if (Player2Paddle && LastPlayer2MoveFrame != GFrameCounter)
	{
		Player2Paddle->SetMoveInput(0.0f);
	}
}
void APongPlayerController::Restart(const FInputActionValue& Value)
{
	if (PongGameMode && PongGameMode->GetMatchState() == EPongMatchState::MatchEnded)
	{
		PongGameMode->RestartMatch();
	}
}


void APongPlayerController::CacheReferences()
{
	PongGameMode = Cast<APongGameMode>(UGameplayStatics::GetGameMode(this));
	if (PongGameMode)
	{
		Player1Paddle = PongGameMode->GetPaddle(EPongPlayer::Player1);
		Player2Paddle = PongGameMode->GetPaddle(EPongPlayer::Player2);
	}
}
