// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ItemDefines.h"
#include "WarriorsItemState.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsItemState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	int32 EquipItemIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	float ItemSwapCoolTime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	float SwapAnimationBlendAmount = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	EItemType EquipItemType = EItemType::None;
};
