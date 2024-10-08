// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ItemDefines.h"
#include "ItemSubsystem.generated.h"

class UDataTable;

UCLASS()
class UItemSubsystem : public UGameInstanceSubsystem	
{
	GENERATED_BODY()

public:
	UItemSubsystem();
	FItemTypeData* GetItemTypeDataTable(EItemType ItemType) const;
	FItemData* GetItemData(EItemType ItemType, const FString& ItemName) const;
private:
	UPROPERTY()
	UDataTable* ItemTypeDataTable;
};