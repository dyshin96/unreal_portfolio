// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ForDisplayWidget.generated.h"

UCLASS()
class UForDisplayWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializeWithItemData(class AItem* AItem);
private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemName;
};