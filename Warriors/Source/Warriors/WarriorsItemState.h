// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ItemDefines.h"
#include "WarriorsItemState.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsItemState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	int32 EquipItemIndex = INDEX_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	float ItemSwapCoolTime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	EItemType EquipItemType = EItemType::None;
};

