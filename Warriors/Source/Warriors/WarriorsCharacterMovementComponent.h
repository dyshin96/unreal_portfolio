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
	float BeforeTurnControllerYaw;
	float BeforeTurnCharacterYaw;
	const float CameraTurnAngle = 50.0f;
	const float CharacterTurnCameraDirTime = 1.0f;
	const float CharacterTurnWaitTime = 1.0f;
	float AccumulateCameraTurnTime = 0.0f;
	bool bTurningToCamera = false;
	bool bStartCameraDirTurnTimer = false;
protected:
	virtual void BeginPlay() override;
public:
	void PerformMovement(float DeltaTime) override;
	void GetNormalizedVelocity(float& OutForwardVelocity, float& OutRightVelocity);
	bool IsTurningToCamera();
	float GetTurnCameraHalfNormalizedValue();
};