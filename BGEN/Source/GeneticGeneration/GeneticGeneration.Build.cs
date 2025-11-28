using UnrealBuildTool;
using System.IO;

public class GeneticGeneration : ModuleRules
{
	public GeneticGeneration(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// --- 1. RUNTIME MODULES ---
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"AIModule",
			"GameplayTasks",
			"InputCore",
			"Json",
			"JsonUtilities",
			"AssetRegistry"
		});
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Projects",
			"UnrealEd",                 // Fixes SAVE_UpdatedEditorInfo
			"BlueprintGraph",
			"AIGraph",
			"GraphEditor",
			"AssetTools",
			"AssetRegistry",
			"EditorScriptingUtilities",
			"Slate",
			"SlateCore",
			"BehaviorTreeEditor"      // Fixes FBehaviorTreeEditorUtils
		});
		
		if (Target.bBuildEditor == true)
		{
			PrivateDependencyModuleNames.Add("BehaviourTreeGraph");
		}
	}
}