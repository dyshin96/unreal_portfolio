// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarriorsLinkedAnimInstance.h"
#include "WarriorsAnimInstanceProxy.h"

UWarriorsLinkedAnimInstance::UWarriorsLinkedAnimInstance()
{
	bUseMainInstanceMontageEvaluationData = true;
}

void UWarriorsLinkedAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	USkeletalMeshComponent* SkelMesh = GetSkelMeshComponent();
	if (IsValid(SkelMesh))
	{
		Parent = Cast<UWarriorsAnimInstance>(GetSkelMeshComponent()->GetAnimInstance());
	}
}

void UWarriorsLinkedAnimInstance::NativeBeginPlay()
{
	//LinkedInstance는 부모의 데이터를 가져와 애니메이션 정보를 주입하기 떄문에 부모가 필수적으로 존재해야합니다. 
	ensure(Parent.IsValid());
	Super::NativeBeginPlay();
}

FAnimInstanceProxy* UWarriorsLinkedAnimInstance::CreateAnimInstanceProxy()
{
	return new FWarriorsAnimInstanceProxy{ this };
}

void UWarriorsLinkedAnimInstance::RefreshStandingMovement()
{
	if (Parent.IsValid())
	{
		Parent->RefreshStandingMovement();
	}
}

void UWarriorsLinkedAnimInstance::RefreshGroundedMovement()
{
	if (Parent.IsValid())
	{
		Parent->RefreshGroundedMovement();
	}
}

void UWarriorsLinkedAnimInstance::RefreshGrounded()
{
	if (Parent.IsValid())
	{
		Parent->RefreshGrounded();
	}
}

void UWarriorsLinkedAnimInstance::RefreshDynamicTransitions()
{
	if (Parent.IsValid())
	{
		Parent->RefreshDynamicTransitions();
	}
}

void UWarriorsLinkedAnimInstance::InitializeGrounded()
{
	if (Parent.IsValid())
	{
		Parent->InitializeGrounded();
	}
}

void UWarriorsLinkedAnimInstance::SetHipsDirection(EWarriorsHipsDirection HipsDirection)
{
	if (Parent.IsValid())
	{
		Parent->GroundedState.HipsDirection = HipsDirection;
	}
}