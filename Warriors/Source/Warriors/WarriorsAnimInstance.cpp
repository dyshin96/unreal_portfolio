// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarriorsAniminstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UWarriorsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	UpdateWarriorForwardVelocity();
	UpdateWarriorRightVelocity();
}

void UWarriorsAnimInstance::UpdateWarriorForwardVelocity()
{
	ACharacter* Character = Cast<ACharacter>(GetOwningActor());
	UCharacterMovementComponent* CharacterMovement = GetOwningActor()->FindComponentByClass<UCharacterMovementComponent>();
	if (IsValid(Character) && IsValid(CharacterMovement))
	{
		AController* Controller = Character->GetController();
		if(IsValid(Controller))
		{
			FVector Velocity = CharacterMovement->GetLastUpdateVelocity();
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			ForwardVelocity = ProjectVectorOntoVector(Velocity, ForwardDirection).Size();
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Character or CharacterMovement is not valid"));
}

void UWarriorsAnimInstance::UpdateWarriorRightVelocity()
{
	ACharacter* Character = Cast<ACharacter>(GetOwningActor());
	UCharacterMovementComponent* CharacterMovement = GetOwningActor()->FindComponentByClass<UCharacterMovementComponent>();
	if (IsValid(Character) && IsValid(CharacterMovement))
	{
		AController* Controller = Character->GetController();
		if (IsValid(Controller))
		{
			FVector Velocity = CharacterMovement->GetLastUpdateVelocity();
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			RightVelocity = ProjectVectorOntoVector(Velocity, ForwardDirection).Size();
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Character or CharacterMovement is not valid"));
}

FVector UWarriorsAnimInstance::ProjectVectorOntoVector(const FVector& A, const FVector& B)
{
    // ���� B�� ũ���� ������ ���մϴ�.
    float BLengthSquared = B.SizeSquared();

    // ���� B�� ũ�Ⱑ 0�� ���, �翵�� �� �����Ƿ�, �����͸� ��ȯ�մϴ�.
    if (BLengthSquared == 0.0f)
    {
        return FVector::ZeroVector;
    }

    // ���� A�� ���� B�� ������ ���մϴ�.
    float DotProduct = FVector::DotProduct(A, B);

    // �翵 ���͸� ����մϴ�.
    FVector Projection = (DotProduct / BLengthSquared) * B;

    return Projection;
}