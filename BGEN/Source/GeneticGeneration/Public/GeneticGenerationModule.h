#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGeneticGeneration, Log, All);

class FGeneticGenerationModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void OnWorldInitialized(UWorld* world, const UWorld::InitializationValues IVS);
	void RunSimulation(UWorld* world);
	void RunBenchmark(UWorld* world);
};