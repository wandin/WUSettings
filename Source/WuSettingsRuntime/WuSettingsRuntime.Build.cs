using UnrealBuildTool;

public class WuSettingsRuntime : ModuleRules
{
    public WuSettingsRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "DeveloperSettings",
                "Projects",
                "AudioMixer",
                "EnhancedInput"
            }
        );
    }
}