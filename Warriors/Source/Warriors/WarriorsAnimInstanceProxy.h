// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstanceProxy.h"
#include "WarriorsAnimInstanceProxy.generated.h"

class UWarriorsAnimInstance;

USTRUCT()
struct FWarriorsAnimInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

	friend class UWarriorsAnimInstance;
	friend class UWarriorsLinkedAnimInstance;
public:
	FWarriorsAnimInstanceProxy() = default;
	explicit FWarriorsAnimInstanceProxy(class UAnimInstance* AnimationInstance);
};
