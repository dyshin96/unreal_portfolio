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
	//LinkedInstance�� �θ��� �����͸� ������ �ִϸ��̼� ������ �����ϱ� ������ �θ� �ʼ������� �����ؾ��մϴ�. 
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