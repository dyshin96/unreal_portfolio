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
	void AddActivatePhysicsCharacter(class ACharacter* Character);
private:
	void UpdateItemStaticMesh();
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:
	UPROPERTY(EditAnywhere)
	bool bForDisplayItem;

	FSimpleMulticastDelegate OnStartAttack;
	FSimpleMulticastDelegate OnEndAttack;

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
private:
	TArray<ACharacter*> ActivatePhysicsCharacters;
};