// Copyright Epic Games, Inc. All Rights Reserved.

#include "ItemSubsystem.h"
#include "Engine/DataTable.h"

UItemSubsystem::UItemSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> ItemTable(TEXT("DataTable'/Game/Warriors/DataTable/ItemTypeDataTable.ItemTypeDataTable'"));
	if (ItemTable.Succeeded())
	{
		ItemTypeDataTable = ItemTable.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find ItemDataTable"));
	}
}

FItemData* UItemSubsystem::GetItemData(EItemType ItemType, const FString& ItemName) const
{
    FItemTypeData* ItemTypeData = GetItemTypeDataTable(ItemType);
    if (ItemTypeData)
    {
		return ItemTypeData->DataTable.LoadSynchronous()->FindRow<FItemData>(FName(ItemName), TEXT(""), true);
    }
    else
    {
		UE_LOG(LogTemp, Error, TEXT("Failed to find DataTable"));
    }
    return nullptr;
}

FItemTypeData* UItemSubsystem::GetItemTypeDataTable(EItemType ItemType) const
{
    if (IsValid(ItemTypeDataTable))
    {
        // 데이터 테이블의 모든 행을 가져옵니다.
        TArray<FItemTypeData*> AllRows;
        ItemTypeDataTable->GetAllRows(TEXT(""), AllRows);

        for (FItemTypeData* Row : AllRows)
        {
            if (Row && Row->ItemType == ItemType)
            {
                return Row;
            }
        }
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find DataTable"));
	}

    return nullptr;
}