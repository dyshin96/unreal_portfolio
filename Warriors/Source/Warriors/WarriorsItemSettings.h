#pragma once

#include "WarriorsItemSettings.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsItemSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	TObjectPtr<UAnimSequenceBase> TwoHandedSwordDrawSequence;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Warriors");
	float TwoHanedSwordDrawDefaultPlayRate = 1.5f;
};
