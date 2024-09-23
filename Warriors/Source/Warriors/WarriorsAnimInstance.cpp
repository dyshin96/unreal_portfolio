// Copyright Epic Games, Inc. All Rights Reserved.

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

	UWarriorsCharacterMovementComponent* MovementComponent = Cast<UWarriorsCharacterMovementComponent>(Character->FindComponentByClass<UCharacterMovementComponent>());
	if (IsValid(MovementComponent))
	{
		MovementComponent->GetNormalizedVelocity(ForwardVelocity, RightVelocity);
		bHorizontalMoving = MovementComponent->IsHorizontalMoving();
		TurningToCameraAlpha = MovementComponent->GetTurnCameraHalfNormalizedValue(DeltaTime);
		bTurningToCamera = MovementComponent->IsTurningToCamera();
		bTurningRight = MovementComponent->IsTurningRight();
		RotateRate = MovementComponent->GetTurnCameraRotateRate();
	}

	Gait = Character->GetGait();
	RefreshGameThreadLocomotionState();
}

void UWarriorsAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	if (!IsValid(Settings) ||!IsValid(Character))
	{
		return;
	}

	RefreshFeet(DeltaTime);
}

FAnimInstanceProxy* UWarriorsAnimInstance::CreateAnimInstanceProxy()
{
	return new FWarriorsAnimInstanceProxy {this};
}

void UWarriorsAnimInstance::RefreshGameThreadLocomotionState()
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
	
	CacheLocomotionState.bMovingSmooth = (CacheLocomotionState.bHasInput && CacheLocomotionState.bHasVelocity) || 
	CacheLocomotionState.HorizontalSpeed > Settings->GeneralSettings.MovingSmoothSpeedThreshold;

	UE_LOG(LogTemp, Warning, TEXT("StrideAmount = %f"), StandingState.StrideBlendAmount);
	UE_LOG(LogTemp, Warning, TEXT("bMovingSmooth = %d"), CacheLocomotionState.bMovingSmooth);
	UE_LOG(LogTemp, Warning, TEXT("HorizontalSpeed = %f"), CacheLocomotionState.HorizontalSpeed);
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
	FeetState.FeetCrossingAmount = FMath::Clamp(GetCurveValue(UWarriorsConstants::FeetCrossingCurveName()), 0.0f, 1.0f);
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
	//PoseState.GroundedAmount = FMath::Clamp(GetCurveValue(Curves, UWarriorsConstants::PoseGroundedCurveName()), 0.0f, 1.0f);
	PoseState.GaitWalkingAmount = FMath::Clamp(PoseState.GaitAmount, 0.0f, 1.0f);
	PoseState.GaitRunningAmount = FMath::Clamp(PoseState.GaitAmount - 1.0f, 0.0f, 1.0f);
	PoseState.GaitSprintingAmount = FMath::Clamp(PoseState.GaitAmount - 2.0f, 0.0f, 1.0f);
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
		VelocityBlend.ForwardAmount = FMath::Clamp(TargetVelocityBlend.X, 0.0f, 1.0f);
		VelocityBlend.BackAmount = FMath::Clamp(TargetVelocityBlend.X, -1.0, 0.0f);
		VelocityBlend.LeftAmount = FMath::Clamp(TargetVelocityBlend.Y, -1.0f, 0.0f);
		VelocityBlend.RightAmount = FMath::Clamp(TargetVelocityBlend.Y, 0.0f, 1.0f);
	}
	else
	{
		//값 변경이 있으면 자연스럽게 보이기 위해 보간합니다. 
		const float DeltaSeconds(GetDeltaSeconds());
		const float InterpSpeed = Settings->GroundedSettings.VelocityBlendInterpolationSpeed;
		VelocityBlend.ForwardAmount = FMath::FInterpTo(VelocityBlend.ForwardAmount, FMath::Clamp(TargetVelocityBlend.X, 0.0f, 1.0f), DeltaSeconds, InterpSpeed);
		VelocityBlend.BackAmount = FMath::FInterpTo(VelocityBlend.BackAmount, FMath::Clamp(TargetVelocityBlend.X, -1.0, 0.0f), DeltaSeconds, InterpSpeed);
		VelocityBlend.LeftAmount = FMath::FInterpTo(VelocityBlend.LeftAmount, FMath::Clamp(TargetVelocityBlend.Y, -1.0f, 0.0f), DeltaSeconds, InterpSpeed);
		VelocityBlend.RightAmount = FMath::FInterpTo(VelocityBlend.RightAmount, FMath::Clamp(TargetVelocityBlend.Y, 0.0f, 1.0f), DeltaSeconds, InterpSpeed);
	}
}

FVector3f UWarriorsAnimInstance::GetRelativeVelocity() const
{
	return FVector3f{CacheLocomotionState.Quat.UnrotateVector(CacheLocomotionState.Velocity)};
}