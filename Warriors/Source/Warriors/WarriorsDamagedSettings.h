#pragma once

#include "WarriorsDamagedSettings.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsDamagedSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	TObjectPtr<UAnimSequence> DefaultDamagedAnim;
};
