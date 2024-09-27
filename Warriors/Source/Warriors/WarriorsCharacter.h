// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WarriorsMovementSettings.h"
#include "WarriorsGameplayTags.h"
#include "Logging/LogMacros.h"
#include "WarriorsLocomotionState.h"
#include "WarriorsCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AWarriorsCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY()
	USkeletalMeshComponent* HeadMesh;
	UPROPERTY()
	USkeletalMeshComponent* HandsMesh;
	UPROPERTY()
	USkeletalMeshComponent* FeetMesh;
	UPROPERTY()
	USkeletalMeshComponent* HairMesh;
	UPROPERTY()
	USkeletalMeshComponent* EyesMesh;
	UPROPERTY()
	USkeletalMeshComponent* LegsMesh;
	UPROPERTY()
	USkeletalMeshComponent* HelmetMesh;
	UPROPERTY()
	USkeletalMeshComponent* ShoulderArmour;
	UPROPERTY()
	USkeletalMeshComponent* Belt;
	UPROPERTY()
	USkeletalMeshComponent* Necklace;
	UPROPERTY()
	USkeletalMeshComponent* Helmet;

private:
#pragma region locomotion
	FWarriorsLocomotionState LocomotionState;

	UPROPERTY(VisibleAnywhere, Category = "State|Warriors Character", Transient)
	FGameplayTag Gait{ WarriorsGaitTags::Walking };

	UPROPERTY(EditAnyWhere, Category = "Setting|Warriors Character", Transient)
	TObjectPtr<UWarriorsMovementSettings> WarriorsMovementSettings;

	FVector_NetQuantizeNormal InputDirection{ForceInit};
#pragma endregion 

	UPROPERTY()
	class UWarriorsCharacterMovementComponent* WarriorsCharacterMovementComponent;
public:
	AWarriorsCharacter(const FObjectInitializer& ObjectInitializer);
private:
	void SetInputDirection(FVector NewInputDirection);
	void RefreshInput(const float DeltaTime);
	void RefreshLocomotionLocationAndRotation();
	void RefreshLocomotion();
	void InitSubMeshs(USkeletalMeshComponent* SkeletalMeshComponent);
	void RefreshGait();
	void SetGait(const FGameplayTag& NewGait);
	FGameplayTag CalculateMaxAllowedGait() const;
	FGameplayTag CalculateActualGait(const FGameplayTag& MaxAllowedGait) const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	void OnGaitChanged(const FGameplayTag& PreviousGait);

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			
	bool SaveSkeletalMeshThumbnailToDisk(class USkeletalMesh* SkeletalMesh, const FString& SavePath);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;
public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FWarriorsLocomotionState GetLocomotionState() const { return LocomotionState; }
	UWarriorsMovementSettings* GetWarriorsMovementSettings() const;
	FGameplayTag GetGait() const { return Gait; };
};

