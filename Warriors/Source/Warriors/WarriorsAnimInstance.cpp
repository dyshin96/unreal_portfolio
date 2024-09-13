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

void UWarriorsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!IsValid(Character))
	{
		return;
	}

	RefreshGameThreadLocomotionState();
	

	UWarriorsCharacterMovementComponent* MovementComponent = Cast<UWarriorsCharacterMovementComponent>(Character->FindComponentByClass<UCharacterMovementComponent>());
	if (IsValid(MovementComponent))
	{
		MovementComponent->GetNormalizedVelocity(ForwardVelocity, RightVelocity);
		bHorizontalMoving = MovementComponent->IsHorizontalMoving();
		TurningToCameraAlpha = MovementComponent->GetTurnCameraHalfNormalizedValue(DeltaSeconds);
		bTurningToCamera = MovementComponent->IsTurningToCamera();
		bTurningRight = MovementComponent->IsTurningRight();
		RotateRate = MovementComponent->GetTurnCameraRotateRate();
	}
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

	// ������ ���� Ŀ���� �ӵ��� �ٿ��ֱ� ���� 
	float Speed = CacheLocomotionState.HorizontalSpeed / CacheLocomotionState.HorizontalScale;
	float BlendRun = Settings->StandingSettings.StrideBlendAmountRunCurve->GetFloatValue(Speed);
	float BlendWalk = Settings->StandingSettings.StrideBlendAmountWalkCurve->GetFloatValue(Speed);
	StandingState.StrideBlendAmount = FMath::Lerp(BlendWalk, BlendRun, PoseState.GaitRunningAmount);
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
		//RelativeVeloicty�� ������ ���밪�� ��� ���� �Ϳ� �� ���е��� ������ ���� ������ ����մϴ�. 
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
		//�� ������ ������ �ڿ������� ���̱� ���� �����մϴ�. 
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