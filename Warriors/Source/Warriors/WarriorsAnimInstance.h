// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WarriorsAnimInstance.generated.h"
UCLASS()
class UWarriorsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	static FVector ProjectVectorOntoVector(const FVector& A, const FVector& B);
protected:
	UFUNCTION(BlueprintCallable)
	float GetWarriorForwardVelocity();
	UFUNCTION(BlueprintCallable)
	float GetWarriorRightVelocity();
};
