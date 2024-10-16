// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WarriorsMovementSettings.h"
#include "WarriorsViewState.h"
#include "WarriorsGameplayTags.h"
#include "WarriorsCharacterSettings.h"
#include "Logging/LogMacros.h"
#include "WarriorsLocomotionState.h"
#include "WarriorsItemState.h"
#include "WarriorsCharacter.generated.h"

class AItem;
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractionAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapItemByWheelAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapItemToFirstAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapItemToSecondAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	UInputAction* UnEquipItemAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	UInputAction* ComboAttackAction;
private:
#pragma region locomotion
	FWarriorsLocomotionState LocomotionState;
	FWarriorsViewState ViewState;
	FWarriorsItemState ItemState;

	UPROPERTY(VisibleAnywhere, Category = "State|Warriors Character", Transient)
	FGameplayTag Gait {WarriorsGaitTags::Walking};

	UPROPERTY(VisibleAnywhere, Category = "State|Warriors LocomotionMode", Transient)
	FGameplayTag LocomotionMode {WarriorsLocomotionModeTags::Grounded};

	UPROPERTY(EditAnyWhere, Category = "Setting|Warriors Character", Transient)
	TObjectPtr<UWarriorsMovementSettings> WarriorsMovementSettings;

	UPROPERTY(EditAnywhere, Category = "Setting|Wariors Character", Transient)
	TObjectPtr<UWarriorsCharacterSettings> Settings;

	FVector_NetQuantizeNormal InputDirection{ForceInit};
#pragma endregion 
	UPROPERTY()
	TArray<AItem*> DetectedItems;
	UPROPERTY()
	TArray<AItem*> GainedItem;
	UPROPERTY()
	AItem* PreviousEquippedItem;
	UPROPERTY()
	class UWarriorsCharacterMovementComponent* WarriorsCharacterMovementComponent;
public:
	AWarriorsCharacter(const FObjectInitializer& ObjectInitializer);
	void SetComboPossible(bool bComboPossible);
private:
	void Gaintem(AItem* Item);
	bool IsCanGain(AItem* Item);
	void EquipItem(int32 Index);
	void UnEquipItem();
	void SetInputDirection(FVector NewInputDirection);
	void SetTargetYawAngle(const float TargetYawAngle);
	void DetectInteractionObject();
	void RefreshViewState(const float DeltaTime);
	void RefreshItem(const float DeltaTime);
	void RefreshInput(const float DeltaTime);
	float CalculateGroundedMovingRotationInterpolationSpeed() const;
	void SetRotationExtraSmooth(const float TargetYawAngle, const float DeltaTime, const float InterpolationSpeed, const float TargetYawAngleRotationSpeed);
	void SetTargetYawAngleSmooth(const float TargetYawAngle, const float DeltaTime, const float RotationSpeed);
	void RefreshTargetYawAngleUsingLocomotionRotation();
	void RefreshLocomotionLocationAndRotation(const float DeltaTime);
	void RefreshLocomotion();
	void RefreshGroundedRotation(const float DeltaTime);
	void ApplyRotationYawSpeedAnimationCurve(float DeltaTime);
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
			
	void OnInteraction(const FInputActionValue& Value);
	void OnSwapItemByWheel(const FInputActionValue& Value);
	void OnSwapItemToFirst(const FInputActionValue& Value);
	void OnSwapItemToSecond(const FInputActionValue& Value);
	void OnUnEquipItem(const FInputActionValue& Value);
	void OnComboAttack(const FInputActionValue& Value);
	bool SaveSkeletalMeshThumbnailToDisk(class USkeletalMesh* SkeletalMesh, const FString& SavePath);
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();
	virtual void PostRegisterAllComponents() override;
	virtual void Tick(float DeltaTime) override;
public:
	FSimpleMulticastDelegate PressComboAttack;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FWarriorsLocomotionState GetLocomotionState() const { return LocomotionState; }
	FWarriorsViewState GetViewState() const { return ViewState; }
	FWarriorsItemState GetItemState() const {return ItemState; }
	UWarriorsMovementSettings* GetWarriorsMovementSettings() const;
	FGameplayTag GetGait() const { return Gait; };
	AItem* GetEquippedItem() const;
};

