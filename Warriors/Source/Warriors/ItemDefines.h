// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDefines.generated.h"

UENUM()
enum class EItemType : uint8
{
	Helmet = 0,
	Armor,
	Pants,
	Shoes,
	Gloves,
	Shoulder,
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
	TSoftObjectPtr<USkeletalMesh> DataTable;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Texture;

};