// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WarriorsConstants.generated.h"

UCLASS(Meta = (BlueprintThreadSafe))
class UWarriorsConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static const FName& TurnInPlaceStandingSlotName();

	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseGaitCurveName();

	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseGroundedCurveName();
};


inline const FName& UWarriorsConstants::TurnInPlaceStandingSlotName()
{
	static const FName SlotName(TEXT("TurnInPlaceStanding"));
	return SlotName;
}

inline const FName& UWarriorsConstants::PoseGaitCurveName()
{
	static const FName CurveName(TEXT("PoseGait"));
	return CurveName;
}

inline const FName& UWarriorsConstants::PoseGroundedCurveName()
{
	static const FName CurveName(TEXT("PoseGrounded"));
	return CurveName;
}