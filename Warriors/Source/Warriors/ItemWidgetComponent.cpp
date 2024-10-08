// Copyright Epic Games, Inc. All Rights Reserved.

#include "ItemWidgetComponent.h"
#include "Engine/GameInstance.h"
#include "ItemSubSystem.h"
#include "Item.h"

void UItemWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	AItem* Item = Cast<AItem>(GetOwner());
	if (IsValid(Item))
	{
		SetupAttachment(Item->GetRootComponent());
		RegisterComponent();
		
		UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>();
		if (IsValid(ItemSubsystem))
		{
			ItemSubsystem->GetItemData(Item->GetItemType(), Item->GetItemName());
			if (IsValid(ForDisPlayWidget))
			{
				SetWidget(ForDisPlayWidget);
			}
		}
	}
}