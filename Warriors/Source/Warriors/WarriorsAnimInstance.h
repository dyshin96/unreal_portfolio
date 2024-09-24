// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WarriorsLocomotionState.h"
#include "WarriorsStandingState.h"
#include "WarriorsGroundState.h"
#include "WarriorsPoseState.h"
#include "WarriorsFeetState.h"
#include "WarriorsGameplayTags.h"
#include "WarriorsAnimationInstanceSettings.h"
#include "WarriorsAnimInstance.generated.h"

class UWarriorsLinkedAnimInstance;

UCLASS()
class UWarriorsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	friend UWarriorsLinkedAnimInstance;
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaTime) override;
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;
private:
	void RefreshLocomotionOnGameThread();
	void RefreshVelocityBlend();
	void RefreshFeet(const float DeltaTime);
	void RefreshRotationYawOffsets(const float ViewRelativeVelocityYawAngle);
	void RefreshMovementDirection(const float ViewRelativeVelocityYawAngle);
private:
	UPROPERTY()
	class AWarriorsCharacter* Character;
	FGameplayTag Gait;

protected:
	FVector3f GetRelativeVelocity() const;
	UFUNCTION(BlueprintCallable, Category = "Warriors|Animation", Meta = (BlueprintThreadSafe))
	void InitializeGrounded();
	UFUNCTION(BlueprintCallable, Category = "Warriors|Animation", Meta = (BlueprintThreadSafe))
	void RefreshGroundedMovement();
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FWarriorsFeetState FeetState;

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
