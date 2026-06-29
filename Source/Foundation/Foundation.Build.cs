using UnrealBuildTool;

public class Foundation : ModuleRules
{
	public Foundation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"NavigationSystem",
			"AIModule",
			"EnhancedInput",
			"Slate",
			"SlateCore",
			"UMG",
			"ImageWrapper"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}
