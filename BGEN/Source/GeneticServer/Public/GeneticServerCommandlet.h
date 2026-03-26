#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "GeneticSimulationManager.h" 
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

	// -- Evolution State --
	int32 PopulationSize = 10;
	int32 CurrentGeneration = 0;
	
	TArray<FSimulationResult> CurrentEpochResults;
	TArray<FSimulationResult> AllTimeResults;

	// Core Genetic Loop
	void GenerateNextEpoch();
	void GenerateInitialEpoch();
	void GenerateSubsequentEpoch();
};