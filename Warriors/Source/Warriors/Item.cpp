// Copyright Epic Games, Inc. All Rights Reserved.

#include "Item.h"
#include "Components/BoxComponent.h"
#include "ItemWidgetComponent.h"
#include "ItemSubSystem.h"

void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (bForDisplayItem)
	{
		ItemWidgetComponent = NewObject<UItemWidgetComponent>(this);
		OnDetected.AddWeakLambda(this, [this]()
		{
			ItemWidgetComponent->SetVisibility(true);
		});

		OnUnDetected.AddWeakLambda(this, [this]()
		{
			ItemWidgetComponent->SetVisibility(false);
		});
	}

	UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>();
	if (IsValid(ItemSubsystem))
	{
		FItemData* ItemData = ItemSubsystem->GetItemData(ItemType, ItemName);
		if (ItemData)
		{
			if (ItemData->StaticMesh.LoadSynchronous())
			{
				StaticMeshComponent = NewObject<UStaticMeshComponent>();
				StaticMeshComponent->SetStaticMesh(ItemData->StaticMesh.Get());
			}
		}
	}
}

bool AItem::IsForDisplayItem()
{
	return bForDisplayItem;
}