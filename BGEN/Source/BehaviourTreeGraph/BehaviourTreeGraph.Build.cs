using UnrealBuildTool;

public class BehaviourTreeGraph : ModuleRules
{
	public BehaviourTreeGraph(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"AIModule",
			"BehaviorTreeEditor" // Essential for graph node classes
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"UnrealEd",
			"GraphEditor",
			"AssetTools",
			"AssetRegistry",
			"AIGraph", // Essential for graph schema
			"EditorFramework",
			"Slate",
			"SlateCore",
			"ToolMenus" 
		});
	}
}