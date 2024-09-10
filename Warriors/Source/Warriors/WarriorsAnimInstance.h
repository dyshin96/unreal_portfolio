// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WarriorsStandingState.h"
#include "WarriorsAnimationInstanceSettings.h"
#include "WarriorsAnimInstance.generated.h"
UCLASS()
class UWarriorsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void RefreshGameThreadLocomotionState();
	void RefreshStandingMovement();

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UWarriorsAnimationInstanceSettings> Settings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FWarriorsStandingState StandingState;

	UPROPERTY(BlueprintReadOnly)
	float ForwardVelocity;
	UPROPERTY(BlueprintReadOnly)
	float RightVelocity;
	UPROPERTY(BlueprintReadOnly)
	bool bTurningToCamera;
	UPROPERTY(BlueprintReadOnly)
	float TurningToCameraAlpha = 0.0f;
	UPROPERTY(BlueprintReadOnly)
	float RotateRate;
	UPROPERTY(BlueprintReadOnly)
	bool bHorizontalMoving;
	UPROPERTY(BlueprintReadOnly)
	bool bTurningRight = false;


};
