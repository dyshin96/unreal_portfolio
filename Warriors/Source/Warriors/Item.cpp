// Copyright Epic Games, Inc. All Rights Reserved.

#include "Item.h"
#include "Components/BoxComponent.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "ItemWidgetComponent.h"
#include "ItemSubSystem.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetVisibility(true);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	RootComponent = StaticMeshComponent;
	bForDisplayItem = false;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	UpdateItemStaticMesh();

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
}

void AItem::InitializeItem(EItemType InItemType, FString InItemName)
{
	ItemType = InItemType;
	ItemName = InItemName;
	UpdateItemStaticMesh();
}

void AItem::UpdateItemStaticMesh()
{
	UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>();
	if (IsValid(ItemSubsystem) && !ItemName.IsEmpty())
	{
		FItemData* ItemData = ItemSubsystem->GetItemData(ItemType, ItemName);
		if (ItemData)
		{
			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			StreamableManager.RequestAsyncLoad(ItemData->StaticMesh.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this, [this, ItemData]()
			{
				if (IsValid(ItemData->StaticMesh.Get()))
				{
					StaticMeshComponent->SetStaticMesh(ItemData->StaticMesh.Get());
				}
			}));
		}
	}
}

bool AItem::IsForDisplayItem()
{
	return bForDisplayItem;
}