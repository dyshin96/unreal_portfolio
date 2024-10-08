﻿#pragma once

#include "WarriorsRotateInPlaceState.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsRotateInPlaceState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bUpdatedThisFrame : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bRotatingLeft : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bRotatingRight : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "x"))
	float PlayRate{1.0f};
};
