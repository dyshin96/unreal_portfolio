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

	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& HipsDirectionLockCurveName();

	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FeetCrossingCurveName();

	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& TransitionSlotName();
	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& AllowTransitionsCurveName();
	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootPlantedCurveName();
	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootLeftIkCurveName();
	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootRightIkCurveName();
	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootLeftLockCurveName();
	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootRightLockCurveName();
	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PelvisBoneName();
	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootLeftIkBoneName();
	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootRightIkBoneName();
	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootLeftVirtualBoneName();
	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootRightVirtualBoneName();
	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& RotationYawSpeedCurveName();
	UFUNCTION(BlueprintPure, Category = "Warriors|Constants|Animation Slots", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName & RotationYawOffsetCurveName();
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

inline const FName& UWarriorsConstants::HipsDirectionLockCurveName()
{
	static const FName Name{ TEXTVIEW("HipsDirectionLock") };
	return Name;
}

inline const FName& UWarriorsConstants::FeetCrossingCurveName()
{
	static const FName Name{ TEXTVIEW("FeetCrossing") };
	return Name;
}

inline const FName& UWarriorsConstants::TransitionSlotName()
{
	static const FName Name{ TEXTVIEW("Transition") };
	return Name;
}

inline const FName& UWarriorsConstants::AllowTransitionsCurveName()
{
	static const FName Name{ TEXTVIEW("AllowTransitions") };
	return Name;
}

inline const FName& UWarriorsConstants::FootPlantedCurveName()
{
	static const FName Name{ TEXTVIEW("FootPlanted") };
	return Name;
}

inline const FName& UWarriorsConstants::FootLeftIkCurveName()
{
	static const FName Name{ TEXTVIEW("FootLeftIk") };
	return Name;
}

inline const FName& UWarriorsConstants::FootRightIkCurveName()
{
	static const FName Name{ TEXTVIEW("FootRightIk") };
	return Name;
}

inline const FName& UWarriorsConstants::FootLeftLockCurveName()
{
	static const FName Name{ TEXTVIEW("FootLeftLock") };
	return Name;
}

inline const FName& UWarriorsConstants::FootRightLockCurveName()
{
	static const FName Name{ TEXTVIEW("FootRightLock") };
	return Name;
}

inline const FName& UWarriorsConstants::PelvisBoneName()
{
	static const FName Name{ TEXTVIEW("pelvis") };
	return Name;
}

inline const FName& UWarriorsConstants::FootLeftIkBoneName()
{
	static const FName Name{ TEXTVIEW("ik_foot_l") };
	return Name;
}

inline const FName& UWarriorsConstants::FootRightIkBoneName()
{
	static const FName Name{ TEXTVIEW("ik_foot_r") };
	return Name;
}

inline const FName& UWarriorsConstants::FootLeftVirtualBoneName()
{
	static const FName Name{ TEXTVIEW("VB foot_l") };
	return Name;
}

inline const FName& UWarriorsConstants::FootRightVirtualBoneName()
{
	static const FName Name{ TEXTVIEW("VB foot_r") };
	return Name;
}

inline const FName& UWarriorsConstants::RotationYawSpeedCurveName()
{
	static const FName Name{ TEXTVIEW("RotationYawSpeed") };
	return Name;
}

inline const FName& UWarriorsConstants::RotationYawOffsetCurveName()
{
	static const FName Name{ TEXTVIEW("RotationYawSpeed") };
	return Name;
}