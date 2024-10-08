// Copyright Epic Games, Inc. All Rights Reserved.

#include "ForDisplayWidget.h"
#include "Item.h"
#include "Components/TextBlock.h"

void UForDisplayWidget::InitializeWithItemData(AItem* AItem)
{
	ItemName->SetText(FText::FromString(AItem->GetItemName()));
}