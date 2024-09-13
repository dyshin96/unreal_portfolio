// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstanceProxy.h"
#include "WarriorsAnimInstance.generated.h"

class UWarriorsAnimInstance;

USTRUCT()
class FWarriorsAnimInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

	friend class UWarriorsAnimInstance;

public:
	explicit FWarriorsAnimInstanceProxy(class UAnimInstance* AnimationInstance);
};
