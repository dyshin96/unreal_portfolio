// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "WarriorsLocomotionState.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsLocomotionState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	uint8 bHasInput : 1{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	float InputYawAngle{ 0.0f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	uint8 bHasVelocity : 1{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	float HorizontalSpeed{ 0.0f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	FVector Velocity{ ForceInit };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	FVector PreviousVelocity{ ForceInit };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	float VelocityYawAngle{0.0f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	uint8 bMoving : 1{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	FVector Location{ForceInit};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	FRotator Rotation{ForceInit};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	FQuat Quat{ForceInit};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	float HorizontalScale {1.0f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	uint8 bMovingSmooth : 1{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	float TargetYawAngle{0.0f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	float SmoothTargetYawAngle{ 0.0f };
};
