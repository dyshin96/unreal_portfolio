// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "WarriorsMovementGaitSettings.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsMovementGaitSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float WalkForwardSpeed{ 150.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float WalkBackwardSpeed{ 350.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float RunForwardSpeed{ 375.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float RunBackwardSpeed{ 375.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float SprintSpeed{ 650.0f };

public:
	float GetMaxWalkSpeed() const;
	float GetMaxRunSpeed() const;
};


inline float FWarriorsMovementGaitSettings::GetMaxWalkSpeed() const
{
	return WalkForwardSpeed;
}

inline float FWarriorsMovementGaitSettings::GetMaxRunSpeed() const
{
	return RunForwardSpeed;
}