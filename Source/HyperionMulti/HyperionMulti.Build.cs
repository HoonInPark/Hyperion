// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HyperionMulti : ModuleRules
{
	public HyperionMulti(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });
	}
}
