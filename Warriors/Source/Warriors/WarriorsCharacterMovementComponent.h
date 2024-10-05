// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WarriorsMovementSettings.h"
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
	float HalfNormalizedTurnCameraAlpha = 0.5f;
	FGameplayTag CurrentRotationMode;

	UPROPERTY(VisibleAnywhere, Category = "State", Transient, Meta = (ClampMin = 0, ClampMax = 3))
	float GaitAmount = 0.0f;


	UPROPERTY(VisibleAnywhere, Category = "State")
	FGameplayTag MaxAllowedGait = WarriorsGaitTags::Running;
	UPROPERTY(VisibleAnywhere, Category = "State", Transient)
	FWarriorsMovementGaitSettings GaitSettings;
	UPROPERTY(VisibleAnywhere, Category = "Settings", Transient)
	TObjectPtr<UWarriorsMovementSettings> MovementSettings;

	UPROPERTY(VisibleAnywhere, Category = "State")
	float MaxAccelerationWalking = 0.0f;
private:
	void CharacterTurnCamera(float DeltaTime);
protected:
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	virtual void PhysWalking(const float DeltaTime, int32 IterationCount);
	void RefreshGroundedMovementSettings();
	void RefreshGaitSettings();
public:
	UWarriorsCharacterMovementComponent();
	void PerformMovement(float DeltaTime) override;
	void GetNormalizedVelocity(float& OutForwardVelocity, float& OutRightVelocity);
	float GetGaitAmount();
	FWarriorsMovementGaitSettings& GetGaitSettings();
};