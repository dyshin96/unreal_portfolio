// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarriorsAnimInstanceProxy.h"
#include "WarriorsAnimInstance.h"

FWarriorsAnimInstanceProxy::FWarriorsAnimInstanceProxy(UAnimInstance* AnimationInstance)
	: FAnimInstanceProxy(AnimationInstance)
{
}

void FWarriorsAnimInstanceProxy::PostUpdate(UAnimInstance* AnimationInstance) const 
{
	Super::PostUpdate(AnimationInstance);


	UWarriorsAnimInstance* WarriorsAnimationInstance {Cast<UWarriorsAnimInstance>(AnimationInstance)};
	if (IsValid(WarriorsAnimationInstance))
	{
		WarriorsAnimationInstance->NativePostUpdateAnimation();
	}
}