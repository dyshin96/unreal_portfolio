// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarriorsAniminstance.h"
#include "GameFramework/Character.h"
#include "WarriorsCharacterMovementComponent.h"

void UWarriorsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	ACharacter* Character = Cast<ACharacter>(GetOwningActor());
	if (IsValid(Character))
	{
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

	const float Speed(LocomotionStae.HorizontalSpeed / );
}