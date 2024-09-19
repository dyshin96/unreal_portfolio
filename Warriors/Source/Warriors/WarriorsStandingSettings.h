// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "WarriorsStandingSettings.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsStandingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float AnimatedWalkSpeed{150.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float AnimatedRunSpeed{350.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float AnimatedSprintSpeed{600.0f};

	// Movement speed to stride blend amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	TObjectPtr<UCurveFloat> StrideBlendAmountWalkCurve;

	// Movement speed to stride blend amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	TObjectPtr<UCurveFloat> StrideBlendAmountRunCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float PivotActivationSpeedThreshold{200.0f};
};
