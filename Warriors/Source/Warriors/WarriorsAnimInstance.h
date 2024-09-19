// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WarriorsLocomotionState.h"
#include "WarriorsStandingState.h"
#include "WarriorsGroundState.h"
#include "WarriorsPoseState.h"
#include "WarriorsAnimationInstanceSettings.h"
#include "WarriorsAnimInstance.generated.h"
UCLASS()
class UWarriorsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;
private:
	void RefreshGameThreadLocomotionState();
	void RefreshVelocityBlend();
private:
	class AWarriorsCharacter* Character;

protected:
	FVector3f GetRelativeVelocity() const;

	UFUNCTION(BlueprintCallable, Category = "Warriors|Animation", Meta = (BlueprintThreadSafe))
	void RefreshStandingMovement();
	UFUNCTION(BlueprintCallable, Category = "Warriors|Animation", Meta = (BlueprintThreadSafe))
	void RefreshGrounded();
	UFUNCTION(BlueprintCallable, Category = "Warriors|Animation", Meta = (BlueprintThreadSafe))
	void RefreshPoseState();
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UWarriorsAnimationInstanceSettings> Settings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FWarriorsLocomotionState CacheLocomotionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FWarriorsStandingState StandingState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FWarriorsGroundState GroundedState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FWarriorsPoseState PoseState;

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
