// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemDefines.h"
#include "Item.generated.h"

class UBoxComponent;
class UItemWidgetComponent;

UCLASS()
class AItem : public AActor
{
	GENERATED_BODY()

public:
	AItem();
	virtual void BeginPlay() override;
	
	bool IsForDisplayItem();
	
	EItemType GetItemType() const { return ItemType; }
	FString GetItemName() const {return ItemName; }
	float GetHeight() const {return 0.0f; }

	void InitializeItem(EItemType ItemType, FString ItemName);
private:
	void UpdateItemStaticMesh();

public:
	UPROPERTY(EditAnywhere)
	bool bForDisplayItem;

	FSimpleMulticastDelegate OnDetected;
	FSimpleMulticastDelegate OnUnDetected;
	FSimpleMulticastDelegate OnInteraction;
protected:
	UPROPERTY(EditAnywhere)
    EItemType ItemType;
	UPROPERTY(EditAnywhere)
	FString ItemName;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(VisibleAnywhere)
	UItemWidgetComponent* ItemWidgetComponent;
	UPROPERTY(VisibleAnywhere)
	TArray<UBoxComponent*> BoxComponents;
};