// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarriorsCharacterMovementComponent.h"
#include "GameFramework/Character.h"

void UWarriorsCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	bOrientRotationToMovement = false; // Character moves in the direction of input
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