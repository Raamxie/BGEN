#include "GeneticGenerationModule.h"
#include "GeneticSimulationManager.h"

// Engine Includes
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "Misc/OutputDeviceFile.h"

DEFINE_LOG_CATEGORY(LogGeneticGeneration, Log, All);

void FGeneticGenerationModule::StartupModule()
{
    // 1. Check Command Line
    if (!FParse::Param(FCommandLine::Get(), TEXT("GeneticRun")))
    {
        UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGeneration module skipped (no -GeneticRun flag)."));
        return;
    }

    UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGenerationModule: Startup."));

    // 2. Create the Manager immediately
    // We utilize the Transient Package so it doesn't belong to a specific world (which dies)
    ActiveManager = NewObject<UGeneticSimulationManager>(GetTransientPackage());
    ActiveManager->AddToRoot(); // CRITICAL: Prevent Garbage Collection between map loads

    // 3. Bind the Listener for when an Epoch finishes
    ActiveManager->OnEpochComplete.AddDynamic(this, &FGeneticGenerationModule::OnEpochFinished);

    // 4. Bind the Map Load Listener (Persists for the life of the module)
    FWorldDelegates::OnPostWorldInitialization.AddRaw(this, &FGeneticGenerationModule::OnWorldInitialized);

    // 5. Initialize State
    CurrentEpoch = 0;
    TotalEpochs = 5; // Set your desired count
    bIsRunningGeneticLoop = true;

    // 6. KICKSTART THE LOOP
    // We cannot load the map *instantly* in StartupModule because the Engine isn't fully ready.
    // We defer the first load slightly, or rely on the user launching the editor with the map.
    // If running headless/commandline, we can request a map load:
    
    UE_LOG(LogGeneticGeneration, Warning, TEXT("--- STARTING GENETIC LOOP EPOCH %d ---"), CurrentEpoch);
    // Note: If the editor launches into 'Main', OnWorldInitialized will fire automatically.
}

void FGeneticGenerationModule::OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS)
{
    // Safety Checks
    if (!bIsRunningGeneticLoop) return;
    if (!World) return;
    
    // Filter out preview worlds, material editor worlds, etc.
    if (!World->IsGameWorld()) return; 

    // Filter specific map names if necessary
    if (World->GetName() != TEXT("Main") && World->GetName() != TEXT("Benchmark"))
    {
        return;
    }

    UE_LOG(LogGeneticGeneration, Log, TEXT("World Init Detected: %s. Starting Epoch %d logic."), *World->GetName(), CurrentEpoch);

    // Execute the Simulation Logic
    RunSimulation(World);
}

void FGeneticGenerationModule::RunSimulation(UWorld* World)
{
    if (!ActiveManager) return;

    // Pass the specific world context to the manager
    // The manager will spawn actors, run the timeline, etc.
    if (CurrentEpoch == 0)
    {
        ActiveManager->Init(World);
    }
    else
    {
        ActiveManager->OnLevelReload(World);
    }

    // IMPORTANT: 
    // The Manager code must eventually call `FinishEpoch()` 
    // after X seconds or when all AI die.
}

void FGeneticGenerationModule::OnEpochFinished()
{
    UE_LOG(LogGeneticGeneration, Log, TEXT("--- EPOCH %d COMPLETE ---"), CurrentEpoch);

    // 1. Increment Generation
    CurrentEpoch++;

    // 2. Check Exit Condition
    if (CurrentEpoch >= TotalEpochs)
    {
        UE_LOG(LogGeneticGeneration, Warning, TEXT("Genetic Simulation Complete. Exiting..."));
        bIsRunningGeneticLoop = false;
        
        // Remove Root to allow GC cleanup
        if (ActiveManager) ActiveManager->RemoveFromRoot();

        // If running in command line mode, request engine exit
        FPlatformMisc::RequestExit(false);
        return;
    }

    // 3. RESTART LOOP (The "Wait" Logic)
    // By opening the level again, we reset the world state.
    // When the load finishes, 'OnWorldInitialized' fires again, starting the next step.
    LoadSimulationMap();
}

void FGeneticGenerationModule::LoadSimulationMap()
{
    // We need a World Context to call OpenLevel.
    // Since we are in a module, we can grab the current GWorld (if available) or use the Engine.
    UWorld* CurrentWorld = GEngine->GetWorldContexts()[0].World();
    
    if (CurrentWorld)
    {
        FString MapName = TEXT("Main"); // Or "Benchmark"
        UE_LOG(LogGeneticGeneration, Log, TEXT("Loading Map for Next Epoch..."));
        UGameplayStatics::OpenLevel(CurrentWorld, FName(*MapName));
    }
}

void FGeneticGenerationModule::ShutdownModule()
{
    if (ActiveManager)
    {
        ActiveManager->RemoveFromRoot();
        ActiveManager = nullptr;
    }
}