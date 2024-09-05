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

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (IsValid(Character))
	{
		if (bStartAccumulateCameraTurnTime)
		{
			AccumulateCameraTurnTime += DeltaTime;
			if (AccumulateCameraTurnTime > CharacterTurnWaitTime)
			{
				bIsTurningToCamera = true;
			}
		}

		if (bIsTurningToCamera)
		{
			float CurrentControlYaw = Character->GetControlRotation().Yaw;
			float TotalTime = CharacterTurnWaitTime + CharacterTurnCameraDirTime;
			// DeltaYaw는 현재 캐릭터의 Yaw와 컨트롤러의 Yaw 사이의 최단 경로 각도 차이
			float DeltaYaw = FMath::FindDeltaAngleDegrees(BeforeTurnCharacterYaw, CurrentControlYaw);
			float LerpYaw = FMath::LerpStable(BeforeTurnCharacterYaw, BeforeTurnCharacterYaw + DeltaYaw, (AccumulateCameraTurnTime - CharacterTurnWaitTime) / CharacterTurnCameraDirTime);
			FRotator CharacterRotator = Character->GetActorRotation();
			CharacterRotator.Yaw = LerpYaw;
			Character->SetActorRotation(CharacterRotator);

			if (AccumulateCameraTurnTime >= TotalTime)
			{
				bStartAccumulateCameraTurnTime = false;
				bIsTurningToCamera = false;
				AccumulateCameraTurnTime = 0.0f;
			}
		}
		else
		{
			BeforeTurnCharacterYaw = Character->GetActorRotation().Yaw;
		}

		FVector ForwardVector = Character->GetActorForwardVector();
		FVector ControllerForward = Character->GetControlRotation().Vector();
		float Degrees = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardVector, ControllerForward)));

		if (FMath::Abs(Degrees) > 60.0f)
		{
			bStartAccumulateCameraTurnTime = true;
		}
	}
}

void UWarriorsCharacterMovementComponent::GetNormalizedVelocity(float& OutForwardVelocity, float& OutRightVelocity)
{
	FRotator Rotation = GetLastUpdateRotation();
	FVector LocalVelocity = Rotation.UnrotateVector(LastUpdateVelocity);

	if (FMath::IsNearlyZero(GetMaxSpeed()) == false)
	{
		OutForwardVelocity = LocalVelocity.X / GetMaxSpeed();

		OutRightVelocity = LocalVelocity.Y / GetMaxSpeed();
	}
	else
	{
		OutForwardVelocity = 0.0f;
		OutRightVelocity = 0.0f;
	}
}