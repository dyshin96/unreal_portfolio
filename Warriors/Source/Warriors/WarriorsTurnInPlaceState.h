#pragma once

#include "WarriorsTurnInPlaceState.generated.h"

class UWarriorsTurnInPlaceSettings;

USTRUCT(BlueprintType)
struct FWarriorsTurnInPlaceState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	uint8 bUpdatedThisFrame : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ForceUnits = "s"))
	float ActivationDelay{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	TObjectPtr<UWarriorsTurnInPlaceSettings> QueuedSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	FName QueuedSlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float QueuedTurnYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ForceUnits = "x"))
	float PlayRate{1.0f};
};
