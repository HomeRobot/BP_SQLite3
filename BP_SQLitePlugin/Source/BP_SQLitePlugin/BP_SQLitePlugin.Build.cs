using UnrealBuildTool;

public class BP_SQLitePlugin : ModuleRules
{
    public BP_SQLitePlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new string[] {
                "BP_SQLitePlugin/Public"
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                "BP_SQLitePlugin/Private"
            }
        );
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] 
            {
                "SQLiteCore",
                "BlueprintGraph",
                "KismetCompiler",
                "UnrealEd",
                "Slate",
                "SlateCore",
                "GraphEditor"
            }
        );
    }
}