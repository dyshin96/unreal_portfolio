// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "WarriorsStandingState.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsStandingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Standing", Meta = (ClampMin = 0, ClampMax = 1))
	float StrideBlendAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Standing", Meta = (ClampMin = 0, ClampMax = 1))
	float WalkRunBlendAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Standing", Meta = (ClampMin = 0, ClampMax = 3, ForceUnits = "x"))
	float PlayRate{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Standing", Meta = (ClampMin = 0, ClampMax = 1))
	float SprintBlockAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Standing", Meta = (ClampMin = 0, ForceUnits = "s"))
	float SprintTime{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Standing", Meta = (ClampMin = -1, ClampMax = 1))
	float SprintAccelerationAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Standing")
	uint8 bPivotActive : 1 {false};
};
