// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HyperionBase : ModuleRules
{
	public HyperionBase(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });
	}
}
