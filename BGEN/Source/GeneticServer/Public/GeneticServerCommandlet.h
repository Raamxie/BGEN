#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "GeneticSimulationManager.h" // Needed for FSimulationResult struct
#include "GeneticServerCommandlet.generated.h"

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
	
	// NEW: Sequential ID for jobs
	int32 NextJobID = 1;

	// -- Evolution State --
	int32 PopulationSize = 10;
	int32 CurrentGeneration = 0;
	
	TArray<FSimulationResult> CurrentEpochResults;
	TArray<FSimulationResult> AllTimeResults;

	void GenerateNextEpoch();
};