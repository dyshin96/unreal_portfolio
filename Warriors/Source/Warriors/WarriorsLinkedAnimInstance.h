// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "WarriorsAnimInstance.h"
#include "WarriorsLinkedAnimInstance.generated.h"
UCLASS()
class UWarriorsLinkedAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UWarriorsLinkedAnimInstance();
private:
	UPROPERTY(VisibleAnywhere, Category = "State", Transient)
	TWeakObjectPtr<UWarriorsAnimInstance> Parent;
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;

	UFUNCTION(BlueprintPure, Category = "Warriors|LinkedAnimInstance",
		Meta = (BlueprintThreadSafe, ReturnDisplayName = "Parent"))
	UWarriorsAnimInstance* GetParent() const;

	UFUNCTION(BlueprintCallable, Category = "Warriors|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshStandingMovement();
	UFUNCTION(BlueprintCallable, Category = "Warriors|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshGroundedMovement();

	UFUNCTION(BlueprintCallable, Category = "Warriors|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshGrounded();
	UFUNCTION(BlueprintCallable, Category = "Warriors|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshDynamicTransitions();
	UFUNCTION(BlueprintCallable, Category = "Warriors|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshRotateInPlace();
	UFUNCTION(BlueprintCallable, Category = "Warriors|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeGrounded();
	UFUNCTION(BlueprintCallable, Category = "Warriors|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void SetHipsDirection(EWarriorsHipsDirection HipsDirection);
	UFUNCTION(BlueprintCallable, Category = "Warriors|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshTurnInPlace();
	UFUNCTION(BlueprintCallable, Category = "Warriors|Linked Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeTurnInPlace();
};

inline UWarriorsAnimInstance* UWarriorsLinkedAnimInstance::GetParent() const
{
	return Parent.Get();
}