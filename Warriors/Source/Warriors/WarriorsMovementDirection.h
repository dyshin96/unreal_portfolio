// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "WarriorsMovementDirection.generated.h"

UENUM(BlueprintType)
enum class EWarriorsMovementDirection : uint8
{
	Forward,
	Backward,
	Left,
	Right
};

USTRUCT(BlueprintType)
struct FWarriorsMovementDirectionCache
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bForward : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bBackward : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bLeft : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bRight : 1 {false};

public:
	constexpr FWarriorsMovementDirectionCache() = default;

	// ReSharper disable once CppNonExplicitConvertingConstructor
	constexpr FWarriorsMovementDirectionCache(const EWarriorsMovementDirection MovementDirection)
	{
		bForward = MovementDirection == EWarriorsMovementDirection::Forward;
		bBackward = MovementDirection == EWarriorsMovementDirection::Backward;
		bLeft = MovementDirection == EWarriorsMovementDirection::Left;
		bRight = MovementDirection == EWarriorsMovementDirection::Right;
	}
};
