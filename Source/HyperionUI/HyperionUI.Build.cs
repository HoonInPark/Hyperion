// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HyperionUI : ModuleRules
{
	public HyperionUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"UMG", 
			"Slate", 
			"SlateCore",
			"HyperionBase"
		});
	}
}
