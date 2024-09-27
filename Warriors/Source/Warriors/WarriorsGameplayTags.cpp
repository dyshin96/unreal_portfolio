#include "WarriorsGameplayTags.h"

namespace WarriorsGaitTags
{
	UE_DEFINE_GAMEPLAY_TAG(Walking, FName(TEXT("Warriors.Gait.Walking")));
	UE_DEFINE_GAMEPLAY_TAG(Running, FName(TEXT("Warriors.Gait.Running")));
	UE_DEFINE_GAMEPLAY_TAG(Sprinting, FName(TEXT("Warriors.Gait.Sprinting")));
}

namespace WarriorsRotationModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(ViewDirection, FName{ TEXTVIEW("Warriors.RotationMode.ViewDirection") });
}

namespace WarriorsStanceTags
{
	UE_DEFINE_GAMEPLAY_TAG(Standing, FName{ TEXTVIEW("Warriors.Stance.Standing") });
	UE_DEFINE_GAMEPLAY_TAG(Crouching, FName{ TEXTVIEW("Warriros.Stance.Crouching") });
}
