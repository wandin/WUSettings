// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

using UnrealBuildTool;

public class WUSettingsRuntime : ModuleRules
{
    public WUSettingsRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "DeveloperSettings" 
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Projects",
                "AudioMixer",
                "EnhancedInput"
            }
        );
    }
}