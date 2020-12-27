// Created by Paul baudy

using UnrealBuildTool;
using System.IO;

public class AnimSolversRuntime : ModuleRules
{
	public AnimSolversRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PrecompileForTargets = PrecompileTargetsType.Any;

		PublicDependencyModuleNames.AddRange(
			new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "AnimationCore",
                    "AnimGraphRuntime",
                }
			);
    }
}
