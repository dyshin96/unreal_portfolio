#pragma once

#include "WarriorsCharacterSettings.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UWarriorsCharacterSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float MovingSpeedThreshold{50.0f};
};
