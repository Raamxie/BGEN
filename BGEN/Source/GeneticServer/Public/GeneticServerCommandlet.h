#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "GeneticSimulationManager.h" 
#include "GeneticServerCommandlet.generated.h"

// Struct to track jobs currently being processed by workers
struct FDispatchedJob
{
	FString AssetPath;
	double DispatchTime;
};

UCLASS()
class UGeneticServerCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UGeneticServerCommandlet();
	virtual int32 Main(const FString& Params) override;
	friend class FGeneticServerJobQueueTest;
	friend class FGeneticServerInitialEpochTest;

private:
	// -- Networking / Job State --
	TArray<FString> JobQueue;
	FString GetNextJob();
	
	// Sequential ID for jobs
	int32 NextJobID = 1;

	// -- Timeout / Requeue System --
	TMap<int32, FDispatchedJob> ActiveJobs;
	float JobTimeoutSeconds = 120.0f; // Give workers 2 minutes before assuming they crashed
	void CheckForTimeouts();

	// -- Evolution State --
	int32 PopulationSize;
	int32 CurrentGeneration;
	int32 InitialMutationCount;
	float CrossoverChance;
	float MutationChance;
	
	TArray<FSimulationResult> CurrentEpochResults;
	TArray<FSimulationResult> AllTimeResults;
	
	TSet<FString> EvaluatedHashes;

	void GenerateNextEpoch();
	void GenerateInitialEpoch();
	void GenerateSubsequentEpoch();

	// -- Multi-Trial Evaluation State --
	int32 TrialsPerGenome = 3; 
	TMap<FString, FSimulationResult> PendingEvaluations;

	// The current surviving parent pool (μ)
	TArray<FSimulationResult> SurvivingPopulation;

	// Replaces GenerateNextEpoch to handle the μ+λ selection before generating
	void ProcessCompletedEpoch();

	// Updated CSV Path reference so we can write to it from anywhere
	FString CSVFilePath = FPaths::ProjectSavedDir() / TEXT("GeneticResults.csv");;
	
};