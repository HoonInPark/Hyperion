// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ServerHyperion : ModuleRules
{
    public ServerHyperion(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] {
				// ... add public include paths required here ...
                //Path.Combine(ModuleDirectory, "Public")
            }
            );

        PrivateIncludePaths.AddRange(
            new string[] {
				// ... add other private include paths required here ...
			}
            );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "ServerHyperionLibrary",
                "Projects"
				// ... add other public dependencies that you statically link with here ...
			}
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
				// ... add private dependencies that you statically link with here ...	
			}
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );
    }
}
