// Copyright Epic Games, Inc. All Rights Reserved.

#include "Item.h"
#include "WarriorsCharacter.h"
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
	StaticMeshComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBeginOverlap);
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
			if (IsValid(ItemWidgetComponent))
			{
				ItemWidgetComponent->SetVisibility(true);
			}
		});

		OnUnDetected.AddWeakLambda(this, [this]()
		{
			if (IsValid(ItemWidgetComponent))
			{
				ItemWidgetComponent->SetVisibility(false);
			}
		});
	}	

	OnStartAttack.AddWeakLambda(this, [this]()
	{
		if (IsValid(StaticMeshComponent))
		{
			StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
	});

	OnEndAttack.AddWeakLambda(this, [this]()
	{
		if (IsValid(StaticMeshComponent))
		{
			StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ActivatePhysicsCharacters.Reset();
		}
	});
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

void AItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWarriorsCharacter* Character = Cast<AWarriorsCharacter>(OtherActor);
	if (GetOwner() && GetOwner() != Character && IsValid(Character) && !ActivatePhysicsCharacters.Contains(Character))
	{
		Character->OnBeginOverlap(OtherBodyIndex, StaticMeshComponent->GetForwardVector(), this);
	}
}

void AItem::AddActivatePhysicsCharacter(ACharacter* Character)
{
	ActivatePhysicsCharacters.Add(Character);
}

bool AItem::IsForDisplayItem()
{
	return bForDisplayItem;
}