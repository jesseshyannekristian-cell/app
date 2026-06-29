using UnrealBuildTool;
using System.Collections.Generic;

public class FoundationTarget : TargetRules
{
	public FoundationTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("Foundation");
	}
}
