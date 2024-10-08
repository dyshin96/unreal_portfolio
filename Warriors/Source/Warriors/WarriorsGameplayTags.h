#pragma once

#include "NativeGameplayTags.h"

namespace WarriorsGaitTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Walking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Running);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sprinting);
}

namespace WarriorsRotationModeTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ViewDirection);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Aiming);
}

namespace WarriorsStanceTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Standing);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Crouching);
}

namespace WarriorsLocomotionModeTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Grounded);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InAir);
}