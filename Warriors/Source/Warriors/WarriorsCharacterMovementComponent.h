// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WarriorsCharacterMovementComponent.generated.h"

UCLASS()
class WARRIORS_API UWarriorsCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

private:
	float BeforeTurnCharacterYaw;
	const float CameraTurnAngle = 90.0f;
	const float CharacterTurnCameraDirTime = 1.0f;
	const float CharacterTurnWaitTime = 1.0f;
	float AccumulateCameraTurnTime = 0.0f;
	bool bIsTurningToCamera = false;
	bool bStartAccumulateCameraTurnTime = false;
protected:
	virtual void BeginPlay() override;
public:
	void PerformMovement(float DeltaTime) override;
	void GetNormalizedVelocity(float& OutForwardVelocity, float& OutRightVelocity);


};