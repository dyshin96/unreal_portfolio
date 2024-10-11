// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ItemDefines.h"
#include "WarriorsItemState.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsItemState
{
	GENERATED_BODY()

	EItemType ItemType = EItemType::None;
	bool bHolding = false;
};
