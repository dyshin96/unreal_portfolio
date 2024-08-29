// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WarriorsAccount.generated.h"

USTRUCT()
struct FEquippedItemInfo
{
	GENERATED_BODY()
	FString Helmet;
	FString Armor;
	FString Pants;
	FString Shoes;
};

UCLASS()
class UWarriorsAccount : public USubsystem
{
	GENERATED_BODY()

public:
	UWarriorsAccount();
private:
	FEquippedItemInfo EquippedInfo;
};