// Copyright Epic Games, Inc. All Rights Reserved.

#include "Item.h"
#include "Components/BoxComponent.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "ItemWidgetComponent.h"
#include "ItemSubSystem.h"

void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (bForDisplayItem)
	{
		ItemWidgetComponent = NewObject<UItemWidgetComponent>(this);
		ItemWidgetComponent->SetupAttachment(GetRootComponent());
		ItemWidgetComponent->RegisterComponent();

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
			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			StreamableManager.RequestAsyncLoad(ItemData->StaticMesh.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this, [this, ItemData]() 
			{
				if (ItemData && IsValid(ItemData->StaticMesh.Get()))
				{
					StaticMeshComponent = NewObject<UStaticMeshComponent>(this);
					StaticMeshComponent->SetStaticMesh(ItemData->StaticMesh.Get());
					StaticMeshComponent->SetupAttachment(RootComponent);
					StaticMeshComponent->RegisterComponent();
				}
			}));
		}
	}
}

bool AItem::IsForDisplayItem()
{
	return bForDisplayItem;
}