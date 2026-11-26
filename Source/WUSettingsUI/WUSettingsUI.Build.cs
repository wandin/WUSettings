// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

using UnrealBuildTool;
public class WUSettingsUI : ModuleRules
{
    public WUSettingsUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UMG",
                "WUSettingsRuntime"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore",
                "InputCore",
            }
        );
    }
}