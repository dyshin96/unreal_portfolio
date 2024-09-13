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

FAnimInstanceProxy UWarriorsLinkedAnimInstance::CreateAnimInstanceProxy()
{
	return new FWarriorsAnimInstanceProxy {this};
}