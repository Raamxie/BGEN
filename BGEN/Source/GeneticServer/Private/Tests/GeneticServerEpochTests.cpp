#include "Misc/AutomationTest.h"
#include "GeneticServerCommandlet.h"
#include "Math/UnrealMathUtility.h" 
#include "UObject/UObjectGlobals.h"

// EditorContext flag is required because we are loading and saving real UAssets
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneticServerInitialEpochTest, "GeneticGeneration.Server.InitialEpoch", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGeneticServerInitialEpochTest::RunTest(const FString& Parameters)
{
	// 1. Arrange
	UGeneticServerCommandlet* ServerCommandlet = NewObject<UGeneticServerCommandlet>();
	if (!TestNotNull(TEXT("Commandlet created"), ServerCommandlet)) return false;

	// --- THE MAGIC BULLET FOR DETERMINISM ---
	// By forcing a specific seed, every run of this test will produce the exact same 
	// sequence of mutations, hashes, and files.
	FMath::RandInit(12345);

	// Setup constraints based on your requirements
	ServerCommandlet->PopulationSize = 5;
	ServerCommandlet->CurrentGeneration = 0;
	ServerCommandlet->InitialMutationCount = 3; 

	// 2. Act
	// This invokes the logic to load the Seed tree, duplicate it 5 times, 
	// mutate each independently, save them as assets, and add them to the queue.
	ServerCommandlet->GenerateInitialEpoch();

	// 3. Assert
	
	// Requirement: Generation should be set to 1
	TestEqual(TEXT("Generation should increment to 1"), ServerCommandlet->CurrentGeneration, 1);
	
	// Requirement: 5 jobs ready
	TestEqual(TEXT("Job Queue should have exactly 5 items"), ServerCommandlet->JobQueue.Num(), 5);
	
	// Requirement: No duplicate trees (checked via your internal hash tracking)
	TestEqual(TEXT("Evaluated Hashes should have exactly 5 unique entries"), ServerCommandlet->EvaluatedHashes.Num(), 5);

	// Requirement: 5 trees saved to asset paths and no duplicate paths in queue
	TSet<FString> UniqueJobPaths;
	for (const FString& JobPath : ServerCommandlet->JobQueue)
	{
		UniqueJobPaths.Add(JobPath);
		
		// Verify the asset actually exists in memory (meaning SaveAsNewAsset worked)
		UObject* SavedTree = FindObject<UObject>(nullptr, *JobPath);
		TestNotNull(FString::Printf(TEXT("Tree asset should be saved and findable at path: %s"), *JobPath), SavedTree);
	}
	
	TestEqual(TEXT("There must not be any duplicate file paths in the job queue"), UniqueJobPaths.Num(), 5);

	// Optional: You could use IFileManager::Get().Delete() here to clean up the generated .uasset 
	// files from your hard drive so the test doesn't permanently bloat your project folder,
	// though your SaveAsNewAsset function uses bOverwriteExisting=true, so it's relatively safe.

	return true;
}