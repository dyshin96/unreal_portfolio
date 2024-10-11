// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDefines.generated.h"

constexpr int32 MaxGainedItemCount = 2;

UENUM()
enum class EItemType : uint8
{
	None, //∏«¡÷∏‘ 
	TwoHandedSword,
};

USTRUCT()
struct FItemTypeData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EItemType ItemType;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UDataTable> DataTable;
};

USTRUCT()
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString ItemName;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> StaticMesh;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Texture;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> ForDisPlayWidget;
};