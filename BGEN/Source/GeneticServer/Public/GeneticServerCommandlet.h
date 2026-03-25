#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "GeneticSimulationManager.h" // Needed for FSimulationResult struct
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
	int32 PopulationSize = 10;
	int32 CurrentGeneration = 0;
	
	TArray<FSimulationResult> CurrentEpochResults;
	TArray<FSimulationResult> AllTimeResults;

	void GenerateNextEpoch();
};