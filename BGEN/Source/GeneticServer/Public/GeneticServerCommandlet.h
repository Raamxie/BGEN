#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "GeneticSimulationManager.h"
#include "Containers/Queue.h"
#include "HAL/CriticalSection.h"
#include "GeneticServerCommandlet.generated.h"

// Struct to track jobs currently being processed by workers
struct FDispatchedJob
{
	FString AssetPath;
	double DispatchTime;
};

// Struct to hold submission data from the HTTP background threads safely
struct FWorkerSubmission
{
	FString AssetPath;
	int32 JobID;
	float DamageDealt;
	float DamageTaken;
	float Distance;
	float Utilization;
	int32 TreeSize;
	float Fitness;
	FString TreeString;
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
	// -- Thread Safety & Queue Management --
	FCriticalSection JobStateMutex;
	TQueue<FWorkerSubmission, EQueueMode::Mpsc> SubmissionQueue;
	void ProcessSubmissions();

	// -- Networking / Job State --
	TArray<FString> JobQueue;
	FString GetNextJob();
	
	// Sequential ID for jobs
	int32 NextJobID = 1;

	// -- Timeout / Requeue System --
	TMap<int32, FDispatchedJob> ActiveJobs;
	// INCREASED to 120s: Accommodates long simulations (e.g., 40-60s) without triggering false timeouts.
	float JobTimeoutSeconds = 120.0f; 
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

	bool bIsShuttingDown = false;
	double ShutdownStartTime = 0.0;
	
	// -- Multi-Trial Evaluation State --
	int32 TrialsPerGenome; 
	TMap<FString, FSimulationResult> PendingEvaluations;

	// The current surviving parent pool (μ)
	TArray<FSimulationResult> SurvivingPopulation;

	// Replaces GenerateNextEpoch to handle the μ+λ selection before generating
	void ProcessCompletedEpoch();

	// Updated CSV Path reference so we can write to it from anywhere
	FString CSVFilePath = FPaths::ProjectSavedDir() / TEXT("GeneticResults.csv");
};