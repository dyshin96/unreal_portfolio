// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WarriorsLocomotionState.h"
#include "WarriorsStandingState.h"
#include "WarriorsGroundState.h"
#include "WarriorsPoseState.h"
#include "WarriorsFeetState.h"
#include "WarriorsTransitionsState.h"
#include "WarriorsDynamicTransitionsState.h"
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
public:
	virtual void NativePostUpdateAnimation();
private:
	void RefreshLocomotionOnGameThread();
	void RefreshVelocityBlend();
	void RefreshFeet(const float DeltaTime);
	void RefreshFeetOnGameThread();
	void RefreshRotationYawOffsets(const float ViewRelativeVelocityYawAngle);
	void RefreshMovementDirection(const float ViewRelativeVelocityYawAngle);
	void RefreshTransitions();
	void PlayQueuedTransitionAnimation();
	void StopQueuedTransitionAndTurnInPlaceAnimations();
	void RefreshFoot(FWarriorsFootState& FootState, const FName& IkCurveName, const FName& LockCurveName, const FTransform& ComponentTransformInverse, const float DeltaTime) const;
	void ProcessFootLockTeleport(float IkAmount, FWarriorsFootState& FootState) const;
	void ProcessFootLockBaseChange(float IkAmount, FWarriorsFootState& FootState, const FTransform& ComponentTransformInverse) const;
	void RefreshFootLock(float IkAmount, FWarriorsFootState& FootState, const FName& LockCurveName, const FTransform& ComponentTransformInverse, float DeltaTime) const;
private:
	UPROPERTY()
	class AWarriorsCharacter* Character;
	FGameplayTag Gait;
	FGameplayTag Stance = WarriorsStanceTags::Standing;
protected:
	UPROPERTY(VisibleAnywhere, Category = "State", Transient)
	double TeleportedTime = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	uint8 bPendingUpdate : 1 {true};
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
	UFUNCTION(BlueprintCallable, Category = "Warriors|Animation", Meta = (BlueprintThreadSafe))
	void RefreshDynamicTransitions();

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FWarriorsTransitionsState TransitionsState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FWarriorsDynamicTransitionsState DynamicTransitionsState;
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
public:
	UFUNCTION(BlueprintCallable, Category = "Warriors|Animation Instance", Meta = (BlueprintThreadSafe))
	void PlayTransitionAnimation(UAnimSequenceBase* Sequence, float BlendInDuration = 0.2f, float BlendOutDuration = 0.2f,
	                             float PlayRate = 1.0f, float StartTime = 0.0f, bool bFromStandingIdleOnly = false);
	UFUNCTION(BlueprintCallable, Category = "Warriors|Animation Instance", Meta = (BlueprintThreadSafe))
	void PlayTransitionLeftAnimation(float BlendInDuration = 0.2f, float BlendOutDuration = 0.2f, float PlayRate = 1.0f,
	                                 float StartTime = 0.0f, bool bFromStandingIdleOnly = false);
	UFUNCTION(BlueprintCallable, Category = "Warriors|Animation Instance", Meta = (BlueprintThreadSafe))
	void PlayTransitionRightAnimation(float BlendInDuration = 0.2f, float BlendOutDuration = 0.2f, float PlayRate = 1.0f,
	                                  float StartTime = 0.0f, bool bFromStandingIdleOnly = false);
	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void StopTransitionAndTurnInPlaceAnimations(float BlendOutDuration = 0.2f);
};
