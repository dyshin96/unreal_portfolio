// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SwordMan : ModuleRules
{
	public SwordMan(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
