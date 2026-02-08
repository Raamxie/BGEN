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
#include "GeneticSimulationManager.h"
#include "BehaviourTreeGraph/Public/BehaviourTreeGraphModule.h"
#include "Containers/Ticker.h"
#include "Misc/OutputDeviceFile.h"

DEFINE_LOG_CATEGORY(LogGeneticGeneration);

void FGeneticGenerationModule::StartupModule()
{
    // 1. Validate Command Line
    if (!FParse::Param(FCommandLine::Get(), TEXT("GeneticRun")))
    {
        UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGeneration module skipped (no -GeneticRun flag)."));
        return;
    }

    UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGenerationModule started."));    

    // 2. Setup Logging
    static FOutputDeviceFile GeneticLogFile(*FPaths::Combine(FPaths::ProjectLogDir(), TEXT("GeneticGeneration.log")), true);
    if (!GLog->IsRedirectingTo(&GeneticLogFile))
    {
        GLog->AddOutputDevice(&GeneticLogFile);
    }

    // 3. Create Manager (Persistent)
    ActiveManager = NewObject<UGeneticSimulationManager>(GetTransientPackage());
    ActiveManager->AddToRoot(); // Prevent GC
    
    // 4. Bind Manager -> Module Delegate
    ActiveManager->OnEpochComplete.AddRaw(this, &FGeneticGenerationModule::OnEpochFinished);

    // 5. Bind World Init
    FWorldDelegates::OnPostWorldInitialization.AddRaw(this, &FGeneticGenerationModule::OnWorldInitialized);

    // 6. Initialize State
    CurrentEpoch = 0;
    TotalEpochs = 10; // Set desired amount
    bIsRunningGeneticLoop = true;

    // 7. KICKSTART THE LOOP
    // We cannot load the map instantly here (GEngine might be null during startup).
    // We wait for the Engine to be fully initialized.
    if (GEngine && GEngine->IsInitialized())
    {
        OnEngineInitComplete();
    }
    else
    {
        FCoreDelegates::OnPostEngineInit.AddRaw(this, &FGeneticGenerationModule::OnEngineInitComplete);
    }
}

IMPLEMENT_MODULE(FGeneticGenerationModule, GeneticGeneration)

void FGeneticGenerationModule::OnEngineInitComplete()
{
    UE_LOG(LogGeneticGeneration, Log, TEXT("Engine Init Complete. Kicking off Genetic Loop..."));
    LoadSimulationMap();
}

void FGeneticGenerationModule::LoadSimulationMap()
{
    if (!bIsRunningGeneticLoop) return;

    UWorld* WorldContext = nullptr;
    if (GEngine && GEngine->GetWorldContexts().Num() > 0)
    {
        WorldContext = GEngine->GetWorldContexts()[0].World();
    }

    if (WorldContext)
    {
        const FString MapName = TEXT("Main"); // MUST match your map asset name
        UE_LOG(LogGeneticGeneration, Log, TEXT("Loading Map: %s (Epoch %d)"), *MapName, CurrentEpoch);
        UGameplayStatics::OpenLevel(WorldContext, FName(*MapName));
    }
    else
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("FATAL: Could not find World Context to load map!"));
    }
}

void FGeneticGenerationModule::OnWorldInitialized(UWorld* world, const UWorld::InitializationValues IVS)
{
    // Filter out editor previews, etc.
    if (!world || !world->IsGameWorld()) return;

    // Ensure we are in the correct map
    if (world->GetName() != TEXT("Main") && world->GetName() != TEXT("Benchmark"))
    {
        UE_LOG(LogGeneticGeneration, Log, TEXT("Skipping non-target world: %s"), *world->GetName());
        return;
    }

    UE_LOG(LogGeneticGeneration, Log, TEXT("World %s initialized. Starting Simulation Logic."), *world->GetName());
    RunSimulation(world);
}

void FGeneticGenerationModule::RunSimulation(UWorld* World)
{
    if (!ActiveManager) return;

    UE_LOG(LogGeneticGeneration, Log, TEXT("--- EPOCH %d START ---"), CurrentEpoch);

    // SCENARIO A: First Run
    if (CurrentEpoch == 0)
    {
        ActiveManager->Init(World); // Setup basic settings
        ActiveManager->SetPause(true);
        ActiveManager->PreparePlayer();
        ActiveManager->SpawnEnemies(1);
        ActiveManager->Simulate();
    }
    // SCENARIO B: Reload Run
    else
    {
        // Manager keeps its member variables (GenerationCount, BestFitness),
        // but needs the new World pointer.
        ActiveManager->OnLevelReload(World);
    }
}

void FGeneticGenerationModule::OnEpochFinished()
{
    UE_LOG(LogGeneticGeneration, Log, TEXT("--- EPOCH %d COMPLETE ---"), CurrentEpoch);

    // 1. Increment State
    CurrentEpoch++;
    
    // Update Manager's generation count for the next run
    if (ActiveManager)
    {
        ActiveManager->GenerationCount = CurrentEpoch + 1;
    }

    // 2. Check Exit Condition
    if (CurrentEpoch >= TotalEpochs)
    {
        UE_LOG(LogGeneticGeneration, Warning, TEXT("=== ALL EPOCHS COMPLETE ==="));
        bIsRunningGeneticLoop = false;
        
        // Cleanup
        if (ActiveManager) 
        {
            ActiveManager->RemoveFromRoot();
            ActiveManager = nullptr;
        }

        // Optional: Exit Application if running in command line
        // FPlatformMisc::RequestExit(false);
    }
    else
    {
        // 3. RESTART LOOP
        // Loading the map again destroys the current world and triggers OnWorldInitialized when done.
        LoadSimulationMap();
    }
}

void FGeneticGenerationModule::ShutdownModule()
{
    if (ActiveManager)
    {
        ActiveManager->RemoveFromRoot();
        ActiveManager = nullptr;
    }
    
    UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGenerationModule shut down."));
}