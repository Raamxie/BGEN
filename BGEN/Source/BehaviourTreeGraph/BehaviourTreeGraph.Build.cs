using UnrealBuildTool;

public class BehaviourTreeGraph : ModuleRules
{
    public BehaviourTreeGraph(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        

        PublicDependencyModuleNames.AddRange(new string[]
        {
	        "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "BehaviorTreeEditor"
        });
        PrivateDependencyModuleNames.AddRange(new string[] {
	        "BehaviorTreeEditor",
	        "GraphEditor",
	        "UnrealEd",
	        "AssetTools",
	        "AssetRegistry",
	        "EditorSubsystem",
	        "EditorFramework",
	        "AIGraph"
        });
    }
}