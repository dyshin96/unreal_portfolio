#pragma once

#include "WarriorsItemSettings.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsItemSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	TObjectPtr<UAnimMontage> DrawSwordMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors")
	TObjectPtr<UAnimMontage> ComboAttackMontage;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Warriors");
	float DrawSwordMontagePlayRate = 1.5f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Warriors");
	float DefaultComboActivetime = 1.0f;
};
