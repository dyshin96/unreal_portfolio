// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ItemDefines.h"
#include "WarriorsItemState.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsItemState
{
	GENERATED_BODY()

	int32 EquipItemIndex = 0;
	EItemType EquipItemType = EItemType::None;
};
