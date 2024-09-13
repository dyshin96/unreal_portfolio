// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "WarriorsGroundState.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsVelocityBlendState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	uint8 bInitializationRequired : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 1))
	float ForwardAmount{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 1))
	float BackAmount{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 1))
	float LeftAmount{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 1))
	float RightAmount{ 0.0f };
};

USTRUCT(BlueprintType)
struct FWarriorsGroundState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = -1, ClampMax = 1))
	float HipsDirectionLockAmount{0.0f};

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	//EWarriorsHipsDirection HipsDirection{EWarriorsHipsDirection::Forward};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	FWarriorsVelocityBlendState VelocityBlend;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	//FWarriorsMovementDirectionCache MovementDirection;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	//FWarriorsRotationYawOffsetsState RotationYawOffsets;
};
