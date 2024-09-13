#pragma once

#include "WarriorsGroundedSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct FWarriorsGroundedSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	TObjectPtr<UCurveFloat> RotationYawOffsetForwardCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	TObjectPtr<UCurveFloat> RotationYawOffsetBackwardCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	TObjectPtr<UCurveFloat> RotationYawOffsetLeftCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	TObjectPtr<UCurveFloat> RotationYawOffsetRightCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0))
	float VelocityBlendInterpolationSpeed{12.0f};
};
