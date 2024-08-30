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
    // 벡터 B의 크기의 제곱을 구합니다.
    float BLengthSquared = B.SizeSquared();

    // 벡터 B의 크기가 0인 경우, 사영할 수 없으므로, 영벡터를 반환합니다.
    if (BLengthSquared == 0.0f)
    {
        return FVector::ZeroVector;
    }

    // 벡터 A와 벡터 B의 내적을 구합니다.
    float DotProduct = FVector::DotProduct(A, B);

    // 사영 벡터를 계산합니다.
    FVector Projection = (DotProduct / BLengthSquared) * B;

    return Projection;
}