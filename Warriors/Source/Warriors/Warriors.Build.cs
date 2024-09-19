// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Warriors : ModuleRules
{
	public Warriors(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayTags" });

        PrivateDependencyModuleNames.AddRange(new string[] { "ImageWrapper" });

        // UnrealEd 모듈 추가 (에디터 전용)
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }
    }
}
