// Created by Paul baudy

using UnrealBuildTool;

public class AnimSolversEditor : ModuleRules
{
	public AnimSolversEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PrecompileForTargets = PrecompileTargetsType.Any;

		PublicDependencyModuleNames.AddRange(
			new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"AnimGraph",
					"BlueprintGraph",
				
					"AnimSolversRuntime",
				}
			);
	}
}
