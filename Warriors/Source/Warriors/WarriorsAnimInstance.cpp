// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarriorsAniminstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UWarriorsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);



}

float UWarriorsAnimInstance::GetWarriorForwardVelocity()
{
	ACharacter* Character = Cast<ACharacter>(GetOwningActor());
	UCharacterMovementComponent* CharacterMovement = GetOwningActor()->FindComponentByClass<UCharacterMovementComponent>();
	if (IsValid(Character) && IsValid(CharacterMovement))
	{
		FVector Velocity = CharacterMovement->GetLastUpdateVelocity();
		// find out which way is forward
		const FRotator Rotation = Character->GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		FVector ForwardVelocity = ProjectVectorOntoVector(Velocity, ForwardDirection);
		return ForwardVelocity.Size();
	}

	UE_LOG(LogTemp, Error, TEXT("Character or CharacterMovement is not valid"));
	return 0.0f;
}

float UWarriorsAnimInstance::GetWarriorRightVelocity()
{
	ACharacter* Character = Cast<ACharacter>(GetOwningActor());
	UCharacterMovementComponent* CharacterMovement = GetOwningActor()->FindComponentByClass<UCharacterMovementComponent>();
	if (IsValid(Character) && IsValid(CharacterMovement))
	{
		FVector Velocity = CharacterMovement->GetLastUpdateVelocity();
		// find out which way is forward
		const FRotator Rotation = Character->GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		FVector ForwardVelocity = ProjectVectorOntoVector(Velocity, ForwardDirection);
		return ForwardVelocity.Size();
	}
	
	UE_LOG(LogTemp, Error, TEXT("Character or CharacterMovement is not valid"));
	return 0.0f;
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