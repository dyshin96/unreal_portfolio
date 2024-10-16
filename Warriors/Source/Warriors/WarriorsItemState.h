// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ItemDefines.h"
#include "WarriorsItemState.generated.h"

//캐릭터의 현재 아이템 상태를 관리하는 구조체
USTRUCT(BlueprintType)
struct FWarriorsItemState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	int32 EquipItemIndex = INDEX_NONE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	float ItemSwapCoolTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	EItemType EquipItemType = EItemType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	float ItemComboCoolTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	int32 ComboIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	bool bComboPossible = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	bool bComboPressed = false;
};

