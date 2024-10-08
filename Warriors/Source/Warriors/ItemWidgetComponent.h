// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "ItemWidgetComponent.generated.h"

UCLASS()
class UItemWidgetComponent : public UWidgetComponent 
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
private:
	UPROPERTY()
	UUserWidget* ForDisPlayWidget;
};