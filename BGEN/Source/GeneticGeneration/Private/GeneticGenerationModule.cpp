#include "GeneticGenerationModule.h"

#include "CustomBehaviourTree.h"
#include "Engine/Engine.h" 
#include "Engine/World.h"
#include "Engine/GameEngine.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "HAL/PlatformMisc.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "BehaviourTreeGraph/Public/BehaviourTreeGraphModule.h"
#include "Containers/Ticker.h"
#include "Misc/OutputDeviceFile.h"

DEFINE_LOG_CATEGORY(LogGeneticGeneration);

void FGeneticGenerationModule::StartupModule()
{
    if (!FParse::Param(FCommandLine::Get(), TEXT("GeneticRun")))
    {
        UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGeneration module skipped (no -GeneticRun flag)."));
        return;
    }

    UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGenerationModule started."));    

    static FOutputDeviceFile GeneticLogFile(*FPaths::Combine(FPaths::ProjectLogDir(), TEXT("GeneticGeneration.log")), true);

    // Add it as a global output device
    if (!GLog->IsRedirectingTo(&GeneticLogFile))
    {
        GLog->AddOutputDevice(&GeneticLogFile);

        UE_LOG(LogGeneticGeneration, Log, TEXT("Custom log file initialized at %s"), GeneticLogFile.GetFilename());

    }
    
    UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGenerationModule started."));

     FWorldDelegates::OnPostWorldInitialization.AddRaw(this, &FGeneticGenerationModule::OnWorldInitialized);
}

void FGeneticGenerationModule::ShutdownModule()
{
    UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGenerationModule shutting down."));
}

IMPLEMENT_MODULE(FGeneticGenerationModule, GeneticGeneration)


void FGeneticGenerationModule::OnWorldInitialized(UWorld* world, const UWorld::InitializationValues IVS)
{
    UE_LOG(LogGeneticGeneration, Log, TEXT("OnWorldInitialized called for world: %s"), *world->GetName());

    if (!world)
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("OnWorldInitialized called with null world."));
        return;
    }
    
    if (world->GetName() != TEXT("Main") && world->GetName() != TEXT("Benchmark"))
    {
        UE_LOG(LogGeneticGeneration, Log, TEXT("Skipping non-main world: %s"), *world->GetName());
        return;
    }

    // Simulate delayed execution manually (since no loop runs)
    UE_LOG(LogGeneticGeneration, Log, TEXT("World %s initialized, running simulation now."), *world->GetName());

	RunSimulation(world);
}



void FGeneticGenerationModule::RunSimulation(UWorld* world)
{
	UE_LOG(LogTemp, Warning, TEXT("GeneticGeneration: Simulation starting..."));

	// Create generator object
	UCustomBehaviourTree* Generator = NewObject<UCustomBehaviourTree>();
	if (!Generator)
	{
		UE_LOG(LogTemp, Error, TEXT("GeneticGeneration: Failed to create CustomBehaviourTree"));
		return;
	}

	// LOAD ORIGINAL TREE
	const FString SourceBT = TEXT("/Game/Actors/EnemyUnleashed/BT_EnemyUnleashed.BT_EnemyUnleashed");
	if (!Generator->LoadBehaviorTree(SourceBT))
	{
		UE_LOG(LogTemp, Error, TEXT("GeneticGeneration: Failed to load BT %s"), *SourceBT);
		return;
	}

	// MUTATE
	if (!Generator->MutateTree_AddSequenceWithAttack())
	{
		UE_LOG(LogTemp, Error, TEXT("GeneticGeneration: Mutation failed"));
		return;
	}

	// SAVE NEW ASSET
	const FString Destination = TEXT("/Game/BehaviourTrees/Generated/BT_Test_Evolved");
	if (!Generator->SaveAsNewAsset(Destination))
	{
		UE_LOG(LogTemp, Error, TEXT("GeneticGeneration: Save failed"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("GeneticGeneration: Successfully saved evolved BT to %s"), *Destination);

	// Load editor module
	if (FModuleManager::Get().IsModuleLoaded("BehaviourTreeGraph"))
	{
		auto& GraphModule = FModuleManager::LoadModuleChecked<FBehaviourTreeGraphModule>("BehaviourTreeGraph");
		GraphModule.BuildGraphForBehaviorTree(Destination);
	}
}