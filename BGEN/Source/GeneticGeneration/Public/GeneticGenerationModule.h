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

	void LoadSimulationMap();
	void LoadEmptyWaitingMap();

private:
	// Event Callbacks
	void OnWorldInitialized(UWorld* world, const UWorld::InitializationValues IVS);
	void OnEngineInitComplete(); 
	void OnEpochFinished();      
	void OnEndFrame();           // Explicit Yield Callback

	// Logic
	void RunSimulation(UWorld* World);

	// State (No UPROPERTY here, as this is not a UCLASS)
	UGeneticSimulationManager* ActiveManager = nullptr;

	int32 CurrentEpoch = 0;
	int32 TotalEpochs = 5;
	bool bIsRunningGeneticLoop = false;
	
	// Flag to tell the engine to sleep
	bool bIsWaitingForJob = false; 
};