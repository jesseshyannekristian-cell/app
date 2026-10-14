using UnrealBuildTool;
using System.Collections.Generic;

public class FoundationSiteOverseerEditorTarget : TargetRules
{
	public FoundationSiteOverseerEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		ExtraModuleNames.Add("FoundationSiteOverseer");
	}
}