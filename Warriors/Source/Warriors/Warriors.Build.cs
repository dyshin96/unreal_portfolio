// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Warriors : ModuleRules
{
	public Warriors(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayTags" });

        PrivateDependencyModuleNames.AddRange(new string[] { "ImageWrapper" });

        // UnrealEd ��� �߰� (������ ����)
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }
    }
}
