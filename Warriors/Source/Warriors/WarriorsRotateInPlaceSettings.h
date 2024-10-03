#pragma once

#include "WarriorsRotateInPlaceSettings.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsRotateInPlaceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float ViewYawAngleThreshold{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0))
	FVector2f ReferenceViewYawSpeed{180.0f, 460.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0))
	FVector2f PlayRate{1.15f, 3.0f};
};
