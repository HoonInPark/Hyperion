// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HyperionAnim : ModuleRules
{
	public HyperionAnim(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
            "Hyperion"
        });
	}
}
