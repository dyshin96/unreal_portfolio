// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WarriorConstants.generated.h"

UCLASS(Meta = (BlueprintThreadSafe))
class WARRIORS_API UWarriorConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static const FName& TurnInPlaceStandingSlotName();
};


inline const FName& UWarriorConstants::TurnInPlaceStandingSlotName()
{
	static const FName SlotName(TEXT("TurnInPlaceStanding"));
	return SlotName;
}