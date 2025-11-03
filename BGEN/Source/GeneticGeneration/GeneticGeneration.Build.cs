using UnrealBuildTool;

public class GeneticGeneration : ModuleRules
{
	public GeneticGeneration(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"UnrealEd",        // For Commandlets
			"AIModule",        // For Behavior Trees
			"GameplayTasks",
			"AssetTools", // For Asset Management
			"AssetRegistry", // For Asset Management
			"EditorScriptingUtilities" // For Commandlets
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Projects",
			"Slate",
			"SlateCore"
		});
	}
}