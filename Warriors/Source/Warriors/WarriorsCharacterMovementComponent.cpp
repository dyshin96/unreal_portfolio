// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarriorsCharacterMovementComponent.h"
#include "Curves/CurveVector.h"
#include "WarriorsCharacter.h"

UWarriorsCharacterMovementComponent::UWarriorsCharacterMovementComponent()
{
	CurrentRotationMode = WarriorsRotationModeTags::ViewDirection;
}

void UWarriorsCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	bOrientRotationToMovement = false; // Character moves in the direction of input
	bWantsInitializeComponent = true;
}

void UWarriorsCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	AWarriorsCharacter* Character = Cast<AWarriorsCharacter>(GetOwner());
	if (IsValid(Character))
	{
		MovementSettings = Character->GetWarriorsMovementSettings();
		RefreshGaitSettings();
	}
}

void UWarriorsCharacterMovementComponent::PhysWalking(const float DeltaTime, int32 IterationCount)
{
	RefreshGroundedMovementSettings();
	Super::PhysWalking(DeltaTime, IterationCount);
}

void UWarriorsCharacterMovementComponent::RefreshGroundedMovementSettings()
{
	float WalkSpeed = GaitSettings.WalkForwardSpeed;
	float RunSpeed = GaitSettings.RunForwardSpeed;

	if (IsValid(MovementSettings) && Velocity.SizeSquared() > UE_KINDA_SMALL_NUMBER)
	{
		const AController* Controller = {GetController()};
		const auto ViewRotation{
			IsValid(Controller)
				? GetController()->GetControlRotation()
				: GetCharacterOwner()->GetViewRotation()
		};
	
		
		//카메라 로테이션에서 중력 반대방향축으로의 사영 벡터를 구하여 수평으로만 이루어진 로테이터를 구한다. 
		FQuat Quat = ViewRotation.Quaternion();
		const auto Projection{ (-GetGravityDirection() | FVector{Quat.X, Quat.Y, Quat.Z}) * -GetGravityDirection() };
		const FQuat RelativeRotation = FQuat{ Projection.X, Projection.Y, Projection.Z, Quat.W }.GetNormalized();

		//카메라 공간에서 수평 Velocity로 변환합니다. 카메라가 바라보는 방향으로 이동하는 편이 더 자연스러운 이유입니다. 
		const FVector2D RelativeVeloicty{RelativeRotation.UnrotateVector(Velocity)};
		const auto VelocityAngle{ FMath::RadiansToDegrees(FMath::Atan2(RelativeVeloicty.Y, RelativeVeloicty.X))};

		//VelocityAngle을 0에서 1사이의 값을 만들어줍니다.
		const auto ForwardSpeedAmount{
			FMath::GetMappedRangeValueClamped(MovementSettings->VelocityAngleToSpeedInterpolationRange,
											  {1.0f, 0.0f}, FMath::Abs(VelocityAngle))
		};

		//현재 Angle 상태를 이용하여 앞으로 가고 있는지 뒤로 가고 있는지 판단해 RunSpeed와 WalkSpeed를 결정합니다. -> ALS에서 이 코드가 필요한지 잘 모르겠음 -_- 
		WalkSpeed = FMath::Lerp(GaitSettings.WalkBackwardSpeed, GaitSettings.WalkForwardSpeed, ForwardSpeedAmount);
		RunSpeed = FMath::Lerp(GaitSettings.RunBackwardSpeed, GaitSettings.RunForwardSpeed, ForwardSpeedAmount);
	}

	const float Speed{UE_REAL_TO_FLOAT(Velocity.Size2D())};
	//#todo sprint 구현시 추가 필요 
	
	if (Speed > WalkSpeed)
	{
		GaitAmouont = FMath::GetMappedRangeValueClamped(FVector2f{WalkSpeed, RunSpeed}, {1.0f, 2.0f}, Speed);
	}
	else
	{
		GaitAmouont = FMath::GetMappedRangeValueClamped(FVector2f{0.0f, WalkSpeed}, {0.0f, 1.0f}, Speed);
	}

	if (MaxAllowedGait == WarriorsGaitTags::Walking)
	{
		MaxWalkSpeed = WalkSpeed;
	}
	else if (MaxAllowedGait == WarriorsGaitTags::Running)
	{
		MaxWalkSpeed = RunSpeed;
	}

	if (IsValid(GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve))
	{
		const auto& AccelerationAndDecelerationAndGroundFrictionCurves{ GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve->FloatCurves };

		MaxAccelerationWalking = AccelerationAndDecelerationAndGroundFrictionCurves[0].Eval(GaitAmouont);
		GroundFriction = AccelerationAndDecelerationAndGroundFrictionCurves[1].Eval(GaitAmouont);
		BrakingDecelerationWalking = AccelerationAndDecelerationAndGroundFrictionCurves[2].Eval(GaitAmouont); 
	}
}

void UWarriorsCharacterMovementComponent::RefreshGaitSettings()
{
	if (IsValid(MovementSettings) && MovementSettings->RotationModes.Contains(CurrentRotationMode))
	{
		if (MovementSettings->RotationModes[CurrentRotationMode].Stances.Contains(WarriorsStanceTags::Standing))
		{
			GaitSettings = MovementSettings->RotationModes[CurrentRotationMode].Stances[WarriorsStanceTags::Standing];
		}
	}
}

void UWarriorsCharacterMovementComponent::PerformMovement(float DeltaTime)
{
	Super::PerformMovement(DeltaTime);
	CharacterTurnCamera(DeltaTime);
}

void UWarriorsCharacterMovementComponent::CharacterTurnCamera(float DeltaTime)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (IsValid(Character))
	{
		if (bStartCameraDirTurnTimer)
		{
			AccumulateCameraTurnTime += DeltaTime;
			if (AccumulateCameraTurnTime > CharacterTurnWaitTime)
			{
				bTurningToCamera = true;
			}
		}

		if (bTurningToCamera)
		{
			// DeltaYaw는 현재 캐릭터의 Yaw와 컨트롤러의 Yaw 사이의 최단 경로 각도 차이
			float DeltaYaw = FMath::FindDeltaAngleDegrees(BeforeTurnCharacterYaw, BeforeTurnControllerYaw);
			float LerpYaw = FMath::LerpStable(BeforeTurnCharacterYaw, BeforeTurnCharacterYaw + DeltaYaw, (AccumulateCameraTurnTime - CharacterTurnWaitTime) / CharacterTurnCameraDirTime);
			FRotator CharacterRotator = Character->GetActorRotation();
			CharacterRotator.Yaw = LerpYaw;
			Character->SetActorRotation(CharacterRotator);

			float TotalTime = CharacterTurnWaitTime + CharacterTurnCameraDirTime;
			if (AccumulateCameraTurnTime >= TotalTime)
			{
				bStartCameraDirTurnTimer = false;
				bTurningToCamera = false;
				AccumulateCameraTurnTime = 0.0f;
			}
		}
		else
		{
			BeforeTurnControllerYaw = Character->GetControlRotation().Yaw;
			BeforeTurnCharacterYaw = Character->GetActorRotation().Yaw;
		}

		FVector ForwardVector = Character->GetActorForwardVector();
		FVector ControllerForward = Character->GetControlRotation().Vector();
		float Degrees = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardVector, ControllerForward)));

		if (FMath::Abs(Degrees) > CameraTurnAngle)
		{
			bStartCameraDirTurnTimer = true;
		}
	}
}

void UWarriorsCharacterMovementComponent::GetNormalizedVelocity(float& OutForwardVelocity, float& OutRightVelocity)
{
	FRotator Rotation = GetLastUpdateRotation();
	FVector LocalVelocity = Rotation.UnrotateVector(LastUpdateVelocity);

	if (FMath::IsNearlyZero(GetMaxSpeed()) == false)
	{
		OutForwardVelocity = LocalVelocity.X ;
		OutRightVelocity = LocalVelocity.Y;
	}
	else
	{
		OutForwardVelocity = 0.0f;
		OutRightVelocity = 0.0f;
	}
}

bool UWarriorsCharacterMovementComponent::IsTurningToCamera()
{
	return bTurningToCamera;
}

bool UWarriorsCharacterMovementComponent::IsTurningRight()
{
	float DeltaYaw = FMath::FindDeltaAngleDegrees(BeforeTurnCharacterYaw, BeforeTurnControllerYaw);
	return DeltaYaw > 0.0f;
}

bool UWarriorsCharacterMovementComponent::IsHorizontalMoving()
{
	float ForwardVel;
	float RightVel;
	GetNormalizedVelocity(ForwardVel, RightVel);
	float Horizontal = FMath::Abs(ForwardVel) + FMath::Abs(RightVel);
	return Horizontal > GetMaxSpeed() / 3.0f;
}

float UWarriorsCharacterMovementComponent::GetTurnCameraHalfNormalizedValue(float DeltaTime)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (IsValid(Character))
	{
		float MaxDifferYaw = 50.0f;
		float CurrentCharacterYaw = Character->GetActorRotation().Yaw;
		float CurrentControlYaw = Character->GetControlRotation().Yaw;
		float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentCharacterYaw, CurrentControlYaw);
		float HalfNormalizedYaw = FMath::Clamp((DeltaYaw + MaxDifferYaw), 0.0f, 100.0f) / (2.0f * MaxDifferYaw);
		float InterpSpeed = 2.0f;
		return HalfNormalizedTurnCameraAlpha = FMath::FInterpTo(HalfNormalizedTurnCameraAlpha, HalfNormalizedYaw, DeltaTime, InterpSpeed);
	}
	return 0.0f;
}

float UWarriorsCharacterMovementComponent::GetTurnCameraRotateRate()
{
	if (bTurningToCamera)
	{
		float RotateRate = (AccumulateCameraTurnTime - CharacterTurnWaitTime) / CharacterTurnCameraDirTime;
		UE_LOG(LogTemp, Warning, TEXT("RotateRate : %f"), RotateRate);
		return RotateRate;
	}
	return 0.0f;
}

inline FWarriorsMovementGaitSettings& UWarriorsCharacterMovementComponent::GetGaitSettings()
{
	return GaitSettings;
}