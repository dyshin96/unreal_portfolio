#pragma once

#include "WarriorsFeetSettings.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsFeetSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	uint8 bDisableFootLock : 1 {false};

	// Specifies the maximum angle by which the foot lock location can differ from the thigh
	// bone axis. Used to prevent legs from twisting into a spiral when the actor rotates quickly.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float FootLockAngleLimit{90.0f};
};
