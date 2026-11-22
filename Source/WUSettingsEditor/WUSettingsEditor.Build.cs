using UnrealBuildTool;

public class WUSettingsEditor : ModuleRules
{
    public WUSettingsEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UnrealEd"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore",
                "EditorSubsystem",
                "AssetTools",
                "ToolMenus",
                "WUSettingsRuntime",
                "WUSettingsUI"
            }
        );
    }
}