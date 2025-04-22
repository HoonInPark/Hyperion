// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class HyperionServerTarget : TargetRules
{
	public HyperionServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("Hyperion");
		ExtraModuleNames.Add("HyperionBase");
		ExtraModuleNames.Add("HyperionMulti");
	}
}
