// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarriorsAniminstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UWarriorsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	GetVelocity(ForwardVelocity, RightVelocity);
}

void UWarriorsAnimInstance::GetVelocity(float& OutForwardVelocity, float& OutRightVelocity)
{
	ACharacter * Character = Cast<ACharacter>(GetOwningActor());
	UCharacterMovementComponent* CharacterMovement = GetOwningActor()->FindComponentByClass<UCharacterMovementComponent>();
	if (IsValid(Character) && IsValid(CharacterMovement))
	{
		FVector Velocity = CharacterMovement->GetLastUpdateVelocity();
		FRotator Rotation = CharacterMovement->GetLastUpdateRotation();

		FVector LocalVelocity = Rotation.UnrotateVector(Velocity);

		if (FMath::IsNearlyZero(CharacterMovement->GetMaxSpeed()) == false)
		{
			OutForwardVelocity = LocalVelocity.X / CharacterMovement->GetMaxSpeed();

			OutRightVelocity = LocalVelocity.Y / CharacterMovement->GetMaxSpeed();
		}
		else
		{
			OutForwardVelocity = 0.0f;
			OutRightVelocity = 0.0f;
		}

		UE_LOG(LogTemp, Warning, TEXT("Vel X = %f, Y = %f "), OutForwardVelocity, OutRightVelocity);
	}
}