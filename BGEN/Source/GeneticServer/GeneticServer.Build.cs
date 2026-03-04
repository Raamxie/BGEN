using UnrealBuildTool;

public class GeneticServer : ModuleRules
{
	// Make sure this name exactly matches the class name and has NO return type (e.g., no 'void')
	public GeneticServer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"HttpServer", // Required for the built-in HTTP Server
			"Json",
			"JsonUtilities"
		});
	}
}