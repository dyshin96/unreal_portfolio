// Copyright Epic Games, Inc. All Rights Reserved.

#include "ItemWidgetComponent.h"
#include "Engine/GameInstance.h"
#include "ItemSubSystem.h"
#include "ForDisplayWidget.h"
#include "Item.h"

void UItemWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	SetVisibility(false);
	SetCastShadow(false);
	
	AItem* Item = Cast<AItem>(GetOwner());
	if (IsValid(Item))
	{	
		UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>();
		if (IsValid(ItemSubsystem))
		{
			FItemData* ItemData = ItemSubsystem->GetItemData(Item->GetItemType(), Item->GetItemName());
			if (ItemData && ItemData->ForDisPlayWidget)
			{
				SetWidgetClass(ItemData->ForDisPlayWidget.Get());
				UForDisplayWidget* GettingWidget = Cast<UForDisplayWidget>(GetWidget());
				if (IsValid(GettingWidget))
				{
					GettingWidget->InitializeWithItemData(Item);
				}
			}
		}
	}
}