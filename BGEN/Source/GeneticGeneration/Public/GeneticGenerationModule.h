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

private:
	// Event Callbacks
	void OnWorldInitialized(UWorld* world, const UWorld::InitializationValues IVS);
	void OnEngineInitComplete(); // New Kickstart
	void OnEpochFinished();      // New Loop Step

	// Logic
	void RunSimulation(UWorld* World);
	void LoadSimulationMap();

	// State
	UPROPERTY()
	UGeneticSimulationManager* ActiveManager = nullptr;

	int32 CurrentEpoch = 0;
	int32 TotalEpochs = 5;
	bool bIsRunningGeneticLoop = false;
};