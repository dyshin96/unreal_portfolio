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

FAnimInstanceProxy UWarriorsLinkedAnimInstance::CreateAnimInstanceProxy()
{
	return new FWarriorsAnimInstanceProxy {this};
}