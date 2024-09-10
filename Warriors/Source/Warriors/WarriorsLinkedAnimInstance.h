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
	UFUNCTION(BlueprintPure, Category = "Warriors|LinkedAnimInstance", Meta = (BlueprintThreadSafe, ReturnDisplayName = "Parent"))
	UWarriorsAnimInstance* GetParent() const;
};

inline UWarriorsAnimInstance* UWarriorsLinkedAnimInstance::GetParent() const
{
	return Parent.Get();
}