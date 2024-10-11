#pragma once

#include "WarriorsItemSettings.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsItemSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Warriors")
	TObjectPtr<UAnimSequenceBase> TwoHandedSwordDrawSequence;
	UPROPERTY(EditAnyWhere, Category = "Warriors");
	float TwoHanedSwordDrawDefaultPlayRate = 1.5f;
};
