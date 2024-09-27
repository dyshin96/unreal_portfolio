#pragma once

#include "WarriorsTransitionsState.generated.h"

class UAnimSequenceBase;

USTRUCT(BlueprintType)
struct FWarriorsTransitionsState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	uint8 bTransitionsAllowed : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	TObjectPtr<UAnimSequenceBase> QueuedTransitionSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ForceUnits = "s"))
	float QueuedTransitionBlendInDuration{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ForceUnits = "s"))
	float QueuedTransitionBlendOutDuration{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ForceUnits = "x"))
	float QueuedTransitionPlayRate{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ForceUnits = "s"))
	float QueuedTransitionStartTime{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	uint8 bStopTransitionsQueued : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ForceUnits = "s"))
	float QueuedStopTransitionsBlendOutDuration{0.0f};
};
