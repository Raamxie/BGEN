#pragma once

#include "CoreMinimal.h"
#include "GeneticSimulationManager.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGeneticGeneration, Log, All);

class FGeneticGenerationModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// This handles the "Event Loop" logic
	void OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS);
    
	// Callback caught from the Manager
	UFUNCTION()
	void OnEpochFinished();

private:
	void RunSimulation(UWorld* World);
	void LoadSimulationMap();

	// PERSISTENT POINTER
	// Kept alive via AddToRoot() in CPP
	UGeneticSimulationManager* ActiveManager = nullptr;

	// STATE TRACKING
	int32 CurrentEpoch = 0;
	int32 TotalEpochs = 10; // Configure this as needed
	bool bIsRunningGeneticLoop = false;
};