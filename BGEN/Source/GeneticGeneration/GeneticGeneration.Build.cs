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
		
		#if WITH_EDITOR
		    PrivateDependencyModuleNames.Add("UnrealEd"); // only if you absolutely need editor-only helpers
		#endif
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
	}
}