// Copyright Epic Gmes, Inc. All Rights Reserved.

#include "WarriorsAniminstance.h"
#include "WarriorsConstants.h"
#include "Animation/AnimInstanceProxy.h"
#include "GameFramework/Character.h"
#include "WarriorsCharacter.h"
#include "WarriorsCharacterMovementComponent.h"
#include "WarriorsAnimInstanceProxy.h"

void UWarriorsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<AWarriorsCharacter>(GetOwningActor());
}

void UWarriorsAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	ensure(IsValid(Character));
}

void UWarriorsAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	
	if (!IsValid(Character))
	{
		return;
	}

	auto* Mesh{ GetSkelMeshComponent() };

	if (Mesh->IsUsingAbsoluteRotation() && IsValid(Mesh->GetAttachParent()))
	{
		const auto& ParentTransform{ Mesh->GetAttachParent()->GetComponentTransform() };

		// Manually synchronize mesh rotation with character rotation.

		Mesh->MoveComponent(FVector::ZeroVector, ParentTransform.GetRotation() * Character->GetBaseRotationOffset(), false);

		// Re-cache proxy transforms to match the modified mesh transform.

		const auto& Proxy{ GetProxyOnGameThread<FAnimInstanceProxy>() };
		const_cast<FTransform&>(Proxy.GetComponentTransform()) = Mesh->GetComponentTransform();
		const_cast<FTransform&>(Proxy.GetComponentRelativeTransform()) = Mesh->GetRelativeTransform();
		const_cast<FTransform&>(Proxy.GetActorTransform()) = Character->GetActorTransform();
	}

	UWarriorsCharacterMovementComponent* MovementComponent = Cast<UWarriorsCharacterMovementComponent>(Character->FindComponentByClass<UCharacterMovementComponent>());
	if (IsValid(MovementComponent))
	{
		MovementComponent->GetNormalizedVelocity(ForwardVelocity, RightVelocity);
	}

	Gait = Character->GetGait();
	RefreshViewOnGameThread();
	RefreshLocomotionOnGameThread();
	RefreshFeetOnGameThread();
}

void UWarriorsAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	if (!IsValid(Settings) ||!IsValid(Character))
	{
		return;
	}

	DynamicTransitionsState.bUpdatedThisFrame = false;
	RotateInPlaceState.bUpdatedThisFrame = false;
	TurnInPlaceState.bUpdatedThisFrame = false;

	RefreshPoseState();
	RefreshView();
	RefreshFeet(DeltaTime);
	RefreshTransitions();
}

void UWarriorsAnimInstance::NativePostUpdateAnimation()
{
	if (!IsValid(Settings) || !IsValid(Character))
	{
		return;
	}

	PlayQueuedTransitionAnimation();
	PlayQueuedTurnInPlaceAnimation();
	StopQueuedTransitionAndTurnInPlaceAnimations();

	bPendingUpdate = false;
}

FAnimInstanceProxy* UWarriorsAnimInstance::CreateAnimInstanceProxy()
{
	return new FWarriorsAnimInstanceProxy {this};
}

void UWarriorsAnimInstance::RefreshLocomotionOnGameThread()
{
#if WITH_EDITOR
	if (!IsValid(GetWorld()) || !GetWorld()->IsGameWorld())
	{
		return;
	}
#endif

	if (!IsValid(Settings))
	{
		return;
	}

	CacheLocomotionState = Character->GetLocomotionState();
	CacheLocomotionState.bMovingSmooth = (CacheLocomotionState.bHasInput && CacheLocomotionState.bHasVelocity) ||
		CacheLocomotionState.HorizontalSpeed > Settings->GeneralSettings.MovingSmoothSpeedThreshold;
}

void UWarriorsAnimInstance::RefreshStandingMovement()
{
#if WITH_EDITOR
	if (!IsValid(GetWorld()) || !GetWorld()->IsGameWorld())
	{
		return;
	}
#endif

	if (!IsValid(Settings))
	{
		return;
	}

	// 스케일 값이 커지면 속도를 줄여주기 위함 
	float Speed = CacheLocomotionState.HorizontalSpeed / CacheLocomotionState.HorizontalScale;
	float BlendRun = Settings->StandingSettings.StrideBlendAmountRunCurve->GetFloatValue(Speed);
	float BlendWalk = Settings->StandingSettings.StrideBlendAmountWalkCurve->GetFloatValue(Speed);
	StandingState.StrideBlendAmount = FMath::Lerp(BlendWalk, BlendRun, PoseState.GaitRunningAmount);
	StandingState.WalkRunBlendAmount = Gait == WarriorsGaitTags::Walking ? 0.0f : 1.0f;

	const auto WalkRunSpeedAmount{
	FMath::Lerp(Speed / Settings->StandingSettings.AnimatedWalkSpeed,
				Speed / Settings->StandingSettings.AnimatedRunSpeed,
				PoseState.UnweightedGaitRunningAmount)
	};

	const auto WalkRunSprintSpeedAmount{
		FMath::Lerp(WalkRunSpeedAmount,
					Speed / Settings->StandingSettings.AnimatedSprintSpeed,
					PoseState.UnweightedGaitSprintingAmount)
	};
	StandingState.PlayRate = FMath::Clamp(WalkRunSprintSpeedAmount / StandingState.StrideBlendAmount, UE_KINDA_SMALL_NUMBER, 3.0f);
}

void UWarriorsAnimInstance::RefreshGroundedMovement()
{
	if (!IsValid(Settings))
	{
		return;
	}
	
	GroundedState.HipsDirectionLockAmount = FMath::Clamp(GetCurveValue(UWarriorsConstants::HipsDirectionLockCurveName()), -1.0f, 1.0f);

	const float ViewRelativeVelocityYawAngle = FMath::UnwindDegrees(CacheLocomotionState.VelocityYawAngle);

	RefreshMovementDirection(ViewRelativeVelocityYawAngle);
	RefreshRotationYawOffsets(ViewRelativeVelocityYawAngle);
}

void UWarriorsAnimInstance::RefreshMovementDirection(const float ViewRelativeVelocityYawAngle)
{
	if (!IsValid(Settings))
	{
		return;
	}

	static constexpr float ForwardHalfAngle{ 70.0f };
	static constexpr float AngleThreshold{ 5.0f };

	if (ViewRelativeVelocityYawAngle >= -ForwardHalfAngle - AngleThreshold && ViewRelativeVelocityYawAngle <= ForwardHalfAngle + AngleThreshold)
	{
		GroundedState.MovementDirection = EWarriorsMovementDirection::Forward;
	}
	else if (ViewRelativeVelocityYawAngle >= ForwardHalfAngle - AngleThreshold && ViewRelativeVelocityYawAngle <= 180.0f - ForwardHalfAngle + AngleThreshold)
	{
		GroundedState.MovementDirection = EWarriorsMovementDirection::Right;
	}
	else if (ViewRelativeVelocityYawAngle <= -(ForwardHalfAngle - AngleThreshold) && ViewRelativeVelocityYawAngle >= -(180.0f - ForwardHalfAngle + AngleThreshold))
	{
		GroundedState.MovementDirection = EWarriorsMovementDirection::Left;
	}
	else
	{
		GroundedState.MovementDirection = EWarriorsMovementDirection::Backward;
	}
}

void UWarriorsAnimInstance::RefreshRotationYawOffsets(const float ViewRelativeVelocityYawAngle)
{
	auto& RotationYawOffsets{ GroundedState.RotationYawOffsets };

	RotationYawOffsets.ForwardAngle = Settings->GroundedSettings.RotationYawOffsetForwardCurve->GetFloatValue(ViewRelativeVelocityYawAngle);
	RotationYawOffsets.BackwardAngle = Settings->GroundedSettings.RotationYawOffsetBackwardCurve->GetFloatValue(ViewRelativeVelocityYawAngle);
	RotationYawOffsets.LeftAngle = Settings->GroundedSettings.RotationYawOffsetLeftCurve->GetFloatValue(ViewRelativeVelocityYawAngle);
	RotationYawOffsets.RightAngle = Settings->GroundedSettings.RotationYawOffsetRightCurve->GetFloatValue(ViewRelativeVelocityYawAngle);
}

void UWarriorsAnimInstance::InitializeGrounded()
{
	GroundedState.VelocityBlend.bInitializationRequired = true;
}

void UWarriorsAnimInstance::RefreshGrounded()
{
#if WITH_EDITOR
	if (!IsValid(GetWorld()) || !GetWorld()->IsGameWorld())
	{
		return;
	}
#endif

	if (!IsValid(Settings))
	{
		return;
	}

	RefreshVelocityBlend();
}

void UWarriorsAnimInstance::RefreshFeet(const float DeltaTime)
{
	FeetState.FootPlantedAmount = FMath::Clamp(GetCurveValue(UWarriorsConstants::FootPlantedCurveName()), -1.0f, 1.0f);
	FeetState.FeetCrossingAmount = FMath::Clamp(GetCurveValue(UWarriorsConstants::FeetCrossingCurveName()), 0.0f, 1.0f);

	//로컬 좌표계에서의 Transform 
	const auto ComponentTransformInverse{GetProxyOnAnyThread<FAnimInstanceProxy>().GetComponentTransform().Inverse()};

	RefreshFoot(FeetState.Left, UWarriorsConstants::FootLeftIkCurveName(), UWarriorsConstants::FootLeftLockCurveName(), ComponentTransformInverse, DeltaTime);
	RefreshFoot(FeetState.Right, UWarriorsConstants::FootRightIkCurveName(), UWarriorsConstants::FootRightLockCurveName(), ComponentTransformInverse, DeltaTime);
}

void UWarriorsAnimInstance::RefreshFeetOnGameThread()
{
	check(IsInGameThread())

	const auto* Mesh{ GetSkelMeshComponent() };

	FeetState.PelvisRotation = FQuat4f{ Mesh->GetSocketTransform(UWarriorsConstants::PelvisBoneName(), RTS_Component).GetRotation() };

	const auto FootLeftTargetTransform{
		Mesh->GetSocketTransform(Settings->GeneralSettings.bUseFootIkBones
									 ? UWarriorsConstants::FootLeftIkBoneName()
									 : UWarriorsConstants::FootLeftVirtualBoneName())
	};

	FeetState.Left.TargetLocation = FootLeftTargetTransform.GetLocation();
	FeetState.Left.TargetRotation = FootLeftTargetTransform.GetRotation();

	const auto FootRightTargetTransform{
		Mesh->GetSocketTransform(Settings->GeneralSettings.bUseFootIkBones
									 ? UWarriorsConstants::FootRightIkBoneName()
									 : UWarriorsConstants::FootRightVirtualBoneName())
	};

	FeetState.Right.TargetLocation = FootRightTargetTransform.GetLocation();
	FeetState.Right.TargetRotation = FootRightTargetTransform.GetRotation();
}

void UWarriorsAnimInstance::RefreshFoot(FWarriorsFootState& FootState, const FName& IkCurveName, const FName& LockCurveName,const FTransform& ComponentTransformInverse, const float DeltaTime) const
{
	const auto IkAmount{ FMath::Clamp(GetCurveValue(IkCurveName), 0.0f, 1.0f) };
	ProcessFootLockTeleport(IkAmount, FootState);
	ProcessFootLockBaseChange(IkAmount, FootState, ComponentTransformInverse);
	RefreshFootLock(IkAmount, FootState, LockCurveName, ComponentTransformInverse, DeltaTime);
}

void UWarriorsAnimInstance::ProcessFootLockTeleport(const float IkAmount, FWarriorsFootState& FootState) const
{
	// Due to network smoothing, we assume that teleportation occurs over a short period of time, not
	// in one frame, since after accepting the teleportation event, the character can still be moved for
	// some indefinite time, and this must be taken into account in order to avoid foot lock glitches.

	if (bPendingUpdate || GetWorld()->TimeSince(TeleportedTime) > 0.2f || !FAnimWeight::IsRelevant(IkAmount * FootState.LockAmount))
	{
		return;
	}

	const auto& ComponentTransform{ GetProxyOnAnyThread<FAnimInstanceProxy>().GetComponentTransform() };

	FootState.LockLocation = ComponentTransform.TransformPosition(FVector{ FootState.LockComponentRelativeLocation });
	FootState.LockRotation = ComponentTransform.TransformRotation(FQuat{ FootState.LockComponentRelativeRotation });

	/*if (MovementBase.bHasRelativeLocation)
	{
		const auto BaseRotationInverse{ MovementBase.Rotation.Inverse() };

		FootState.LockMovementBaseRelativeLocation =
			FVector3f{ BaseRotationInverse.RotateVector(FootState.LockLocation - MovementBase.Location) };

		FootState.LockMovementBaseRelativeRotation = FQuat4f{ BaseRotationInverse * FootState.LockRotation };
	}*/
}

void UWarriorsAnimInstance::ProcessFootLockBaseChange(const float IkAmount, FWarriorsFootState& FootState,
	const FTransform& ComponentTransformInverse) const
{
	if ((!bPendingUpdate /*!MovementBase.bBaseChanged*/) || !FAnimWeight::IsRelevant(IkAmount * FootState.LockAmount))
	{
		return;
	}

	if (bPendingUpdate)
	{
		FootState.LockLocation = FootState.TargetLocation;
		FootState.LockRotation = FootState.TargetRotation;
	}

	FootState.LockComponentRelativeLocation = FVector3f{ ComponentTransformInverse.TransformPosition(FootState.LockLocation) };
	FootState.LockComponentRelativeRotation = FQuat4f{ ComponentTransformInverse.TransformRotation(FootState.LockRotation) };

	/*if (MovementBase.bHasRelativeLocation)
	{
		const auto BaseRotationInverse{ MovementBase.Rotation.Inverse() };

		FootState.LockMovementBaseRelativeLocation =
			FVector3f{ BaseRotationInverse.RotateVector(FootState.LockLocation - MovementBase.Location) };

		FootState.LockMovementBaseRelativeRotation = FQuat4f{ BaseRotationInverse * FootState.LockRotation };
	}
	else*/
	{
		FootState.LockMovementBaseRelativeLocation = FVector3f::ZeroVector;
		FootState.LockMovementBaseRelativeRotation = FQuat4f::Identity;
	}
}

void UWarriorsAnimInstance::RefreshFootLock(const float IkAmount, FWarriorsFootState& FootState, const FName& LockCurveName, const FTransform& ComponentTransformInverse, const float DeltaTime) const
{
	auto NewLockAmount{ FMath::Clamp(GetCurveValue(LockCurveName), 0, 1) };

	if (CacheLocomotionState.bMovingSmooth/*LocomotionMode != AlsLocomotionModeTags::Grounded*/)
	{
		// Smoothly disable foot lock if the character is moving or in the air,
		// instead of relying on the curve value from the animation blueprint.

		static constexpr auto MovingDecreaseSpeed{ 5.0f };
		static constexpr auto NotGroundedDecreaseSpeed{ 0.6f };

		NewLockAmount = bPendingUpdate
			? 0.0f
			: FMath::Max(0.0f, FMath::Min(
				NewLockAmount,
				FootState.LockAmount - DeltaTime *
				(CacheLocomotionState.bMovingSmooth ? MovingDecreaseSpeed : NotGroundedDecreaseSpeed)));
	}

	if (Settings->FeetSettings.bDisableFootLock || !FAnimWeight::IsRelevant(IkAmount * NewLockAmount))
	{
		if (FootState.LockAmount > 0.0f)
		{
			FootState.LockAmount = 0.0f;

			FootState.LockLocation = FVector::ZeroVector;
			FootState.LockRotation = FQuat::Identity;

			FootState.LockComponentRelativeLocation = FVector3f::ZeroVector;
			FootState.LockComponentRelativeRotation = FQuat4f::Identity;

			FootState.LockMovementBaseRelativeLocation = FVector3f::ZeroVector;
			FootState.LockMovementBaseRelativeRotation = FQuat4f::Identity;
		}

		FootState.FinalLocation = FVector3f{ ComponentTransformInverse.TransformPosition(FootState.TargetLocation) };
		FootState.FinalRotation = FQuat4f{ ComponentTransformInverse.TransformRotation(FootState.TargetRotation) };
		return;
	}

	const auto bNewAmountEqualOne{ FAnimWeight::IsFullWeight(NewLockAmount) };
	const auto bNewAmountGreaterThanPrevious{ NewLockAmount > FootState.LockAmount };

	// Update the foot lock amount only if the new amount is less than the current amount or equal to 1. This
	// allows the foot to blend out from a locked location or lock to a new location, but never blend in.

	if (bNewAmountEqualOne)
	{
		if (bNewAmountGreaterThanPrevious)
		{
			// If the new foot lock amount is 1 and the previous amount is less than 1, then save the new foot lock location and rotation.

			if (FootState.LockAmount <= 0.9f)
			{
				// Keep the same lock location and rotation when the previous lock
				// amount is close to 1 to get rid of the foot "teleportation" issue.

				FootState.LockLocation = FootState.TargetLocation;
				FootState.LockRotation = FootState.TargetRotation;

				FootState.LockComponentRelativeLocation = FVector3f{ ComponentTransformInverse.TransformPosition(FootState.LockLocation) };
				FootState.LockComponentRelativeRotation = FQuat4f{ ComponentTransformInverse.TransformRotation(FootState.LockRotation) };
			}

			/*if (MovementBase.bHasRelativeLocation)
			{
				const auto BaseRotationInverse{ MovementBase.Rotation.Inverse() };

				FootState.LockMovementBaseRelativeLocation =
					FVector3f{ BaseRotationInverse.RotateVector(FootState.TargetLocation - MovementBase.Location) };

				FootState.LockMovementBaseRelativeRotation = FQuat4f{ BaseRotationInverse * FootState.TargetRotation };
			}
			else*/
			{
				FootState.LockMovementBaseRelativeLocation = FVector3f::ZeroVector;
				FootState.LockMovementBaseRelativeRotation = FQuat4f::Identity;
			}
		}

		FootState.LockAmount = 1.0f;
	}
	else if (!bNewAmountGreaterThanPrevious)
	{
		FootState.LockAmount = NewLockAmount;
	}

	/*if (MovementBase.bHasRelativeLocation)
	{
		FootState.LockLocation = MovementBase.Location +
			MovementBase.Rotation.RotateVector(FVector{ FootState.LockMovementBaseRelativeLocation });

		FootState.LockRotation = MovementBase.Rotation * FQuat{ FootState.LockMovementBaseRelativeRotation };
	}*/

	FootState.LockComponentRelativeLocation = FVector3f{ ComponentTransformInverse.TransformPosition(FootState.LockLocation) };
	FootState.LockComponentRelativeRotation = FQuat4f{ ComponentTransformInverse.TransformRotation(FootState.LockRotation) };

	// Limit the foot lock location so that legs do not twist into a spiral when the actor rotates quickly.

	const auto ComponentRelativeThighAxis{ FeetState.PelvisRotation.RotateVector(FootState.ThighAxis) };
	FVector2f From = FVector2f{ ComponentRelativeThighAxis };
	FVector2f To = FVector2f{ FootState.LockComponentRelativeLocation };
	const auto LockAngle = FMath::RadiansToDegrees(FMath::Acos(From | To)) * FMath::Sign(From ^ To);

	if (FMath::Abs(LockAngle) > Settings->FeetSettings.FootLockAngleLimit + UE_KINDA_SMALL_NUMBER)
	{
		const auto ConstrainedLockAngle{ FMath::Clamp(LockAngle, -Settings->FeetSettings.FootLockAngleLimit, Settings->FeetSettings.FootLockAngleLimit) };
		const FQuat4f OffsetRotation{ FVector3f::UpVector, FMath::DegreesToRadians(ConstrainedLockAngle - LockAngle) };

		FootState.LockComponentRelativeLocation = OffsetRotation.RotateVector(FootState.LockComponentRelativeLocation);
		FootState.LockComponentRelativeRotation = OffsetRotation * FootState.LockComponentRelativeRotation;
		FootState.LockComponentRelativeRotation.Normalize();

		const auto& ComponentTransform{ GetProxyOnAnyThread<FAnimInstanceProxy>().GetComponentTransform() };

		FootState.LockLocation = ComponentTransform.TransformPosition(FVector{ FootState.LockComponentRelativeLocation });
		FootState.LockRotation = ComponentTransform.TransformRotation(FQuat{ FootState.LockComponentRelativeRotation });

		/*if (MovementBase.bHasRelativeLocation)
		{
			const auto BaseRotationInverse{ MovementBase.Rotation.Inverse() };

			FootState.LockMovementBaseRelativeLocation =
				FVector3f{ BaseRotationInverse.RotateVector(FootState.LockLocation - MovementBase.Location) };

			FootState.LockMovementBaseRelativeRotation = FQuat4f{ BaseRotationInverse * FootState.LockRotation };
		}*/
	}

	const auto FinalLocation{ FMath::Lerp(FootState.TargetLocation, FootState.LockLocation, FootState.LockAmount) };

	auto FinalRotation{ FQuat::FastLerp(FootState.TargetRotation, FootState.LockRotation, FootState.LockAmount) };
	FinalRotation.Normalize();

	FootState.FinalLocation = FVector3f{ ComponentTransformInverse.TransformPosition(FinalLocation) };
	FootState.FinalRotation = FQuat4f{ ComponentTransformInverse.TransformRotation(FinalRotation) };
}

void UWarriorsAnimInstance::StopQueuedTransitionAndTurnInPlaceAnimations()
{
	check(IsInGameThread());

	if (!TransitionsState.bStopTransitionsQueued)
	{
		return;
	}

	StopSlotAnimation(TransitionsState.QueuedStopTransitionsBlendOutDuration, UWarriorsConstants::TransitionSlotName());
	StopSlotAnimation(TransitionsState.QueuedStopTransitionsBlendOutDuration, UWarriorsConstants::TurnInPlaceStandingSlotName());
	//StopSlotAnimation(TransitionsState.QueuedStopTransitionsBlendOutDuration, UWarriorsConstants::TurnInPlaceCrouchingSlotName());

	TransitionsState.bStopTransitionsQueued = false;
	TransitionsState.QueuedStopTransitionsBlendOutDuration = 0.0f;

	UE_LOG(LogTemp, Warning, TEXT("Stop Transittion Animation"));
}

void UWarriorsAnimInstance::RefreshPoseState()
{
	const auto& Curves{ GetProxyOnAnyThread<FWarriorsAnimInstanceProxy>().GetAnimationCurves(EAnimCurveType::AttributeCurve) };

	auto GetCurveValue = [](const TMap<FName, float>& Curves, const FName& CurveName) -> float
	{
		const auto* Value {Curves.Find(CurveName)};
		return Value != nullptr ? *Value : 0.0f;
	};

	PoseState.GaitAmount = FMath::Clamp(GetCurveValue(Curves, UWarriorsConstants::PoseGaitCurveName()), 0.0f, 3.0f);
	PoseState.GroundedAmount = FMath::Clamp(GetCurveValue(Curves, UWarriorsConstants::PoseGroundedCurveName()), 0.0f, 1.0f);
	PoseState.GaitWalkingAmount = FMath::Clamp(PoseState.GaitAmount, 0.0f, 1.0f);
	PoseState.GaitRunningAmount = FMath::Clamp(PoseState.GaitAmount - 1.0f, 0.0f, 1.0f);
	PoseState.GaitSprintingAmount = FMath::Clamp(PoseState.GaitAmount - 2.0f, 0.0f, 1.0f);

	PoseState.UnweightedGaitAmount = PoseState.GroundedAmount > UE_SMALL_NUMBER
		? PoseState.GaitAmount / PoseState.GroundedAmount
		: PoseState.GaitAmount;

	PoseState.UnweightedGaitWalkingAmount = FMath::Clamp(PoseState.UnweightedGaitAmount, 0.0f, 1.0f);
	PoseState.UnweightedGaitRunningAmount = FMath::Clamp(PoseState.UnweightedGaitAmount - 1.0f, 0.0f, 1.0f);
	PoseState.UnweightedGaitSprintingAmount = FMath::Clamp(PoseState.UnweightedGaitAmount - 2.0f, 0.0f, 1.0f);
}

void UWarriorsAnimInstance::RefreshTransitions()
{
	TransitionsState.bTransitionsAllowed = FAnimWeight::IsFullWeight(GetCurveValue(UWarriorsConstants::AllowTransitionsCurveName()));
}

void UWarriorsAnimInstance::RefreshView()
{
	ViewState.YawAngle = FMath::UnwindDegrees(UE_REAL_TO_FLOAT(ViewState.Rotation.Yaw - CacheLocomotionState.Rotation.Yaw));
	ViewState.PitchAngle = FMath::UnwindDegrees(UE_REAL_TO_FLOAT(ViewState.Rotation.Pitch - CacheLocomotionState.Rotation.Pitch));

	ViewState.PitchAmount = 0.5f - ViewState.PitchAngle / 180.0f;
}

void UWarriorsAnimInstance::RefreshViewOnGameThread()
{
	check(IsInGameThread());

	const FWarriorsViewState View = Character->GetViewState();
	ViewState.Rotation = View.Rotation;
	ViewState.YawSpeed = View.YawSpeed;
}

void UWarriorsAnimInstance::RefreshDynamicTransitions()
{
#if WITH_EDITOR
	if (!IsValid(GetWorld()) || !GetWorld()->IsGameWorld())
	{
		return;
	}
#endif

	if (DynamicTransitionsState.bUpdatedThisFrame || !IsValid(Settings))
	{
		return;
	}

	DynamicTransitionsState.bUpdatedThisFrame = true;

	if (DynamicTransitionsState.FrameDelay > 0)
	{
		DynamicTransitionsState.FrameDelay -= 1;
		return;
	}

	if (!TransitionsState.bTransitionsAllowed)
	{
		return;
	}

	// Check each foot to see if the location difference between the foot look and its desired / target location
	// exceeds a threshold. If it does, play an additive transition animation on that foot. The currently set
	// transition plays the second half of a 2 foot transition animation, so that only a single foot moves.

	const auto FootLockDistanceThresholdSquared{
		FMath::Square(Settings->DynamicTransitions.FootLockDistanceThreshold * CacheLocomotionState.HorizontalScale)
	};

	const auto FootLockLeftDistanceSquared{ FVector::DistSquared(FeetState.Left.TargetLocation, FeetState.Left.LockLocation) };
	const auto FootLockRightDistanceSquared{ FVector::DistSquared(FeetState.Right.TargetLocation, FeetState.Right.LockLocation) };

	const auto bTransitionLeftAllowed{
		FAnimWeight::IsRelevant(FeetState.Left.LockAmount) && FootLockLeftDistanceSquared > FootLockDistanceThresholdSquared
	};

	const auto bTransitionRightAllowed{
		FAnimWeight::IsRelevant(FeetState.Right.LockAmount) && FootLockRightDistanceSquared > FootLockDistanceThresholdSquared
	};

	if (!bTransitionLeftAllowed && !bTransitionRightAllowed)
	{
		return;
	}

	TObjectPtr<UAnimSequenceBase> DynamicTransitionSequence;

	// If both transitions are allowed, choose the one with a greater lock distance.

	if (!bTransitionLeftAllowed)
	{
		DynamicTransitionSequence = Stance == WarriorsStanceTags::Crouching
			? Settings->DynamicTransitions.CrouchingRightSequence
			: Settings->DynamicTransitions.StandingRightSequence;
	}
	else if (!bTransitionRightAllowed)
	{
		DynamicTransitionSequence = Stance == WarriorsStanceTags::Crouching
			? Settings->DynamicTransitions.CrouchingLeftSequence
			: Settings->DynamicTransitions.StandingLeftSequence;
	}
	else if (FootLockLeftDistanceSquared >= FootLockRightDistanceSquared)
	{
		DynamicTransitionSequence = Stance == WarriorsStanceTags::Crouching
			? Settings->DynamicTransitions.CrouchingLeftSequence
			: Settings->DynamicTransitions.StandingLeftSequence;
	}
	else
	{
		DynamicTransitionSequence = Stance == WarriorsStanceTags::Crouching
			? Settings->DynamicTransitions.CrouchingRightSequence
			: Settings->DynamicTransitions.StandingRightSequence;
	}

	if (IsValid(DynamicTransitionSequence))
	{
		// Block next dynamic transitions for about 2 frames to give the animation blueprint some time to properly react to the animation.

		DynamicTransitionsState.FrameDelay = 2;

		// Animation montages can't be played in the worker thread, so queue them up to play later in the game thread.

		TransitionsState.QueuedTransitionSequence = DynamicTransitionSequence;
		TransitionsState.QueuedTransitionBlendInDuration = Settings->DynamicTransitions.BlendDuration;
		TransitionsState.QueuedTransitionBlendOutDuration = Settings->DynamicTransitions.BlendDuration;
		TransitionsState.QueuedTransitionPlayRate = Settings->DynamicTransitions.PlayRate;
		TransitionsState.QueuedTransitionStartTime = 0.0f;

		if (IsInGameThread())
		{
			PlayQueuedTransitionAnimation();
		}
	}
}

bool UWarriorsAnimInstance::IsRotateInPlaceAllowed()
{
	return RotationMode == WarriorsRotationModeTags::Aiming;
}

void UWarriorsAnimInstance::RefreshRotateInPlace()
{
#if WITH_EDITOR
	if (!IsValid(GetWorld()) || !GetWorld()->IsGameWorld())
	{
		return;
	}
#endif

	if (RotateInPlaceState.bUpdatedThisFrame || !IsValid(Settings))
	{
		return;
	}

	RotateInPlaceState.bUpdatedThisFrame = true;

	if (true)
	{
		RotateInPlaceState.bRotatingLeft = false;
		RotateInPlaceState.bRotatingRight = false;
	}
	else
	{
		RotateInPlaceState.bRotatingLeft = ViewState.YawAngle < -Settings->RotateInPlaceSettings.ViewYawAngleThreshold;
		RotateInPlaceState.bRotatingRight = ViewState.YawAngle > Settings->RotateInPlaceSettings.ViewYawAngleThreshold;
	}

	static constexpr auto PlayRateInterpolationSpeed{ 5.0f };

	if (!RotateInPlaceState.bRotatingLeft && !RotateInPlaceState.bRotatingRight)
	{
		RotateInPlaceState.PlayRate = bPendingUpdate
			? Settings->RotateInPlaceSettings.PlayRate.X
			: FMath::FInterpTo(RotateInPlaceState.PlayRate, Settings->RotateInPlaceSettings.PlayRate.X,
				GetDeltaSeconds(), PlayRateInterpolationSpeed);
		return;
	}

	// If the character should rotate, set the play rate to scale with the view yaw
	// speed. This makes the character rotate faster when moving the camera faster.

	const auto PlayRate{
		FMath::GetMappedRangeValueClamped(Settings->RotateInPlaceSettings.ReferenceViewYawSpeed,
										  Settings->RotateInPlaceSettings.PlayRate, ViewState.YawSpeed)
	};

	RotateInPlaceState.PlayRate = bPendingUpdate
		? PlayRate
		: FMath::FInterpTo(RotateInPlaceState.PlayRate, PlayRate,
			GetDeltaSeconds(), PlayRateInterpolationSpeed);
}

bool UWarriorsAnimInstance::IsTurnInPlaceAllowed()
{
	return RotationMode == WarriorsRotationModeTags::ViewDirection;
}

void UWarriorsAnimInstance::InitializeTurnInPlace()
{
	TurnInPlaceState.ActivationDelay = 0.0f;
}

void UWarriorsAnimInstance::RefreshTurnInPlace()
{
#if WITH_EDITOR
	if (!IsValid(GetWorld()) || !GetWorld()->IsGameWorld())
	{
		return;
	}
#endif

	if (TurnInPlaceState.bUpdatedThisFrame || !IsValid(Settings))
	{
		return;
	}

	TurnInPlaceState.bUpdatedThisFrame = true;

	if (!TransitionsState.bTransitionsAllowed || !IsTurnInPlaceAllowed())
	{
		TurnInPlaceState.ActivationDelay = 0.0f;
		return;
	}

	// Check if the view yaw speed is below the threshold and if the view yaw angle is outside the
	// threshold. If so, begin counting the activation delay time. If not, reset the activation delay
	// time. This ensures the conditions remain true for a sustained time before turning in place.

	if (ViewState.YawSpeed >= Settings->TurnInPlaceSettings.ViewYawSpeedThreshold ||
		FMath::Abs(ViewState.YawAngle) <= Settings->TurnInPlaceSettings.ViewYawAngleThreshold)
	{
		TurnInPlaceState.ActivationDelay = 0.0f;
		return;
	}

	TurnInPlaceState.ActivationDelay = TurnInPlaceState.ActivationDelay + GetDeltaSeconds();

	const auto ActivationDelay{
		FMath::GetMappedRangeValueClamped({Settings->TurnInPlaceSettings.ViewYawAngleThreshold, 180.0f},
										  Settings->TurnInPlaceSettings.ViewYawAngleToActivationDelay,
										  FMath::Abs(ViewState.YawAngle))
	};

	// Check if the activation delay time exceeds the set delay (mapped to the view yaw angle). If so, start a turn in place.

	if (TurnInPlaceState.ActivationDelay <= ActivationDelay)
	{
		return;
	}

	// Select settings based on turn angle and stance.
	auto RemapAngleForCounterClockwiseRotation = [](float Angle) -> float
		{
			if (Angle > 180.0f - 5.0f)
			{
				return Angle - 360.0f;
			}

			return Angle;
		};

	const auto bTurnLeft{ RemapAngleForCounterClockwiseRotation(ViewState.YawAngle) <= 0.0f };

	UWarriorsTurnInPlaceSettings* TurnInPlaceSettings{ nullptr };
	FName TurnInPlaceSlotName;

	if (Stance == WarriorsStanceTags::Standing)
	{
		TurnInPlaceSlotName = UWarriorsConstants::TurnInPlaceStandingSlotName();

		if (FMath::Abs(ViewState.YawAngle) < Settings->TurnInPlaceSettings.Turn180AngleThreshold)
		{
			TurnInPlaceSettings = bTurnLeft
				? Settings->TurnInPlaceSettings.StandingTurn90Left
				: Settings->TurnInPlaceSettings.StandingTurn90Right;
		}
		else
		{
			TurnInPlaceSettings = bTurnLeft
				? Settings->TurnInPlaceSettings.StandingTurn180Left
				: Settings->TurnInPlaceSettings.StandingTurn180Right;
		}
	}
	/*else if (Stance == AlsStanceTags::Crouching)
	{
		TurnInPlaceSlotName = UAlsConstants::TurnInPlaceCrouchingSlotName();

		if (FMath::Abs(ViewState.YawAngle) < Settings->TurnInPlace.Turn180AngleThreshold)
		{
			TurnInPlaceSettings = bTurnLeft
				? Settings->TurnInPlace.CrouchingTurn90Left
				: Settings->TurnInPlace.CrouchingTurn90Right;
		}
		else
		{
			TurnInPlaceSettings = bTurnLeft
				? Settings->TurnInPlace.CrouchingTurn180Left
				: Settings->TurnInPlace.CrouchingTurn180Right;
		}
	}*/

	if (IsValid(TurnInPlaceSettings) && ensure(IsValid(TurnInPlaceSettings->Sequence)))
	{
		// Animation montages can't be played in the worker thread, so queue them up to play later in the game thread.

		TurnInPlaceState.QueuedSettings = TurnInPlaceSettings;
		TurnInPlaceState.QueuedSlotName = TurnInPlaceSlotName;
		TurnInPlaceState.QueuedTurnYawAngle = ViewState.YawAngle;

		if (IsInGameThread())
		{
			PlayQueuedTurnInPlaceAnimation();
		}
	}
}

void UWarriorsAnimInstance::PlayQueuedTurnInPlaceAnimation()
{
	check(IsInGameThread());

	if (TransitionsState.bStopTransitionsQueued || !IsValid(TurnInPlaceState.QueuedSettings))
	{
		return;
	}

	const auto* TurnInPlaceSettings{ TurnInPlaceState.QueuedSettings.Get() };

	PlaySlotAnimationAsDynamicMontage(TurnInPlaceSettings->Sequence, TurnInPlaceState.QueuedSlotName,
		Settings->TurnInPlaceSettings.BlendDuration, Settings->TurnInPlaceSettings.BlendDuration,
		TurnInPlaceSettings->PlayRate, 1, 0.0f);

	// Scale the rotation yaw delta (gets scaled in animation graph) to compensate for play rate and turn angle (if allowed).

	TurnInPlaceState.PlayRate = TurnInPlaceSettings->PlayRate;

	//PlayRate 값으로 RotateYawSpeed값을 Scale하여 더 많은 회전을 하도록 수정합니다. 
	if (TurnInPlaceSettings->bScalePlayRateByAnimatedTurnAngle)
	{
		TurnInPlaceState.PlayRate *= FMath::Abs(TurnInPlaceState.QueuedTurnYawAngle / TurnInPlaceSettings->AnimatedTurnAngle);
	}

	TurnInPlaceState.QueuedSettings = nullptr;
	TurnInPlaceState.QueuedSlotName = NAME_None;
	TurnInPlaceState.QueuedTurnYawAngle = 0.0f;
}

void UWarriorsAnimInstance::PlayQueuedTransitionAnimation()
{
	check(IsInGameThread());

	if (TransitionsState.bStopTransitionsQueued || !IsValid(TransitionsState.QueuedTransitionSequence))
	{
		return;
	}

	PlaySlotAnimationAsDynamicMontage(TransitionsState.QueuedTransitionSequence, UWarriorsConstants::TransitionSlotName(),
		TransitionsState.QueuedTransitionBlendInDuration, TransitionsState.QueuedTransitionBlendOutDuration,
		TransitionsState.QueuedTransitionPlayRate, 1, 0.0f, TransitionsState.QueuedTransitionStartTime);

	TransitionsState.QueuedTransitionSequence = nullptr;
	TransitionsState.QueuedTransitionBlendInDuration = 0.0f;
	TransitionsState.QueuedTransitionBlendOutDuration = 0.0f;
	TransitionsState.QueuedTransitionPlayRate = 1.0f;
	TransitionsState.QueuedTransitionStartTime = 0.0f;
	
	UE_LOG(LogTemp, Warning,TEXT("Play Transittion Animation"));
}

void UWarriorsAnimInstance::RefreshVelocityBlend()
{
	FWarriorsVelocityBlendState& VelocityBlend = GroundedState.VelocityBlend;
	FVector3f RelativeVelocity = GetRelativeVelocity();
	FVector3f TargetVelocityBlend{FVector3f::ZeroVector};

	if (RelativeVelocity.Normalize())
	{	
		//RelativeVeloicty의 성분의 절대값을 모두 더한 것에 각 성분들을 나누어 블랜딩 비율을 계산합니다. 
		TargetVelocityBlend = RelativeVelocity / (FMath::Abs(RelativeVelocity.X) + FMath::Abs(RelativeVelocity.Y) + FMath::Abs(RelativeVelocity.Z));
	}

	if (VelocityBlend.bInitializationRequired)
	{
		VelocityBlend.bInitializationRequired = false;

		VelocityBlend.ForwardAmount = FMath::Clamp(TargetVelocityBlend.X, 0.0f, 1.0f);
		VelocityBlend.BackAmount = FMath::Abs(FMath::Clamp(TargetVelocityBlend.X, -1.0, 0.0f));
		VelocityBlend.LeftAmount = FMath::Abs(FMath::Clamp(TargetVelocityBlend.Y, -1.0f, 0.0f));
		VelocityBlend.RightAmount = FMath::Clamp(TargetVelocityBlend.Y, 0.0f, 1.0f);
	}
	else
	{
		//값 변경이 있으면 자연스럽게 보이기 위해 보간합니다. 
		const float DeltaSeconds(GetDeltaSeconds());
		const float InterpSpeed = Settings->GroundedSettings.VelocityBlendInterpolationSpeed;
		VelocityBlend.ForwardAmount = FMath::FInterpTo(VelocityBlend.ForwardAmount, FMath::Clamp(TargetVelocityBlend.X, 0.0f, 1.0f), DeltaSeconds, InterpSpeed);
		VelocityBlend.BackAmount = FMath::FInterpTo(VelocityBlend.BackAmount, FMath::Abs(FMath::Clamp(TargetVelocityBlend.X, -1.0, 0.0f)), DeltaSeconds, InterpSpeed);
		VelocityBlend.LeftAmount = FMath::FInterpTo(VelocityBlend.LeftAmount, FMath::Abs(FMath::Clamp(TargetVelocityBlend.Y, -1.0f, 0.0f)), DeltaSeconds, InterpSpeed);
		VelocityBlend.RightAmount = FMath::FInterpTo(VelocityBlend.RightAmount, FMath::Clamp(TargetVelocityBlend.Y, 0.0f, 1.0f), DeltaSeconds, InterpSpeed);
	}
}

FVector3f UWarriorsAnimInstance::GetRelativeVelocity() const
{
	return FVector3f{CacheLocomotionState.Quat.UnrotateVector(CacheLocomotionState.Velocity)};
}

void UWarriorsAnimInstance::PlayTransitionAnimation(UAnimSequenceBase* Sequence, const float BlendInDuration, const float BlendOutDuration,
	const float PlayRate, const float StartTime, const bool bFromStandingIdleOnly)
{
	if (bFromStandingIdleOnly && (CacheLocomotionState.bMoving || Stance != WarriorsStanceTags::Standing))
	{
		return;
	}

	// Animation montages can't be played in the worker thread, so queue them up to play later in the game thread.

	TransitionsState.QueuedTransitionSequence = Sequence;
	TransitionsState.QueuedTransitionBlendInDuration = BlendInDuration;
	TransitionsState.QueuedTransitionBlendOutDuration = BlendOutDuration;
	TransitionsState.QueuedTransitionPlayRate = PlayRate;
	TransitionsState.QueuedTransitionStartTime = StartTime;

	if (IsInGameThread())
	{
		PlayQueuedTransitionAnimation();
	}
}

void UWarriorsAnimInstance::PlayTransitionLeftAnimation(const float BlendInDuration, const float BlendOutDuration, const float PlayRate,
	const float StartTime, const bool bFromStandingIdleOnly)
{
	if (!IsValid(Settings))
	{
		return;
	}

	PlayTransitionAnimation(Stance == WarriorsStanceTags::Crouching
		? Settings->TransitionsSettings.CrouchingLeftSequence
		: Settings->TransitionsSettings.StandingLeftSequence,
		BlendInDuration, BlendOutDuration, PlayRate, StartTime, bFromStandingIdleOnly);
}

void UWarriorsAnimInstance::PlayTransitionRightAnimation(const float BlendInDuration, const float BlendOutDuration, const float PlayRate,
	const float StartTime, const bool bFromStandingIdleOnly)
{
	if (!IsValid(Settings))
	{
		return;
	}

	PlayTransitionAnimation(Stance == WarriorsStanceTags::Crouching
		? Settings->TransitionsSettings.CrouchingRightSequence
		: Settings->TransitionsSettings.StandingRightSequence,
		BlendInDuration, BlendOutDuration, PlayRate, StartTime, bFromStandingIdleOnly);
}

void UWarriorsAnimInstance::StopTransitionAndTurnInPlaceAnimations(const float BlendOutDuration)
{
	TransitionsState.bStopTransitionsQueued = true;
	TransitionsState.QueuedStopTransitionsBlendOutDuration = BlendOutDuration;
	UE_LOG(LogTemp, Warning, TEXT("Queue Stop Transittion Animation"));
	if (IsInGameThread())
	{
		StopQueuedTransitionAndTurnInPlaceAnimations();
	}
}