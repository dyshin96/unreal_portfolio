// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Engine/DataTable.h"
#include "WarriorsStandingSettings.h"
#include "WarriorsGroundedSettings.h"
#include "WarriorsAnimationInstanceSettings.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UWarriorsAnimationInstanceSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FWarriorsStandingSettings StandingSettings;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FWarriorsGroundedSettings GroundedSettings;
};
