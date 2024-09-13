#pragma once

#include "WarriorsPoseState.generated.h"

USTRUCT(BlueprintType)
struct FWarriorsPoseState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 1))
	float GroundedAmount{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 1))
	float InAirAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 1))
	float StandingAmount{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 1))
	float CrouchingAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 1))
	float MovingAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 3))
	float GaitAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 1))
	float GaitWalkingAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 1))
	float GaitRunningAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 1))
	float GaitSprintingAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 3))
	float UnweightedGaitAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 1))
	float UnweightedGaitWalkingAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 1))
	float UnweightedGaitRunningAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warriors", Meta = (ClampMin = 0, ClampMax = 1))
	float UnweightedGaitSprintingAmount{0.0f};
};
