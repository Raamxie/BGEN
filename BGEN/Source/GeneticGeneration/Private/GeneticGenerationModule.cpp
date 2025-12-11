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

	//RunSimulation(world);
	RunMutationDemonstration(world);
}



void FGeneticGenerationModule::RunSimulation(UWorld* World)
{
	UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGeneration: World Init Detected."));
	if (!World) return;

	// SCENARIO A: First Run (Manager doesn't exist)
	if (!ActiveManager)
	{
		UE_LOG(LogGeneticGeneration, Log, TEXT("PERSISTENCE: Creating NEW Simulation Manager."));
        
		ActiveManager = NewObject<UGeneticSimulationManager>(GetTransientPackage());
		ActiveManager->AddToRoot(); // Prevent GC
		ActiveManager->Init(World); // Initial Setup
        
		// Start the first run
		ActiveManager->SetPause(true);
		ActiveManager->PreparePlayer();
		ActiveManager->SpawnEnemies(1);
		ActiveManager->Simulate();
	}
	// SCENARIO B: Reload Run (Manager exists and holds data)
	else
	{
		UE_LOG(LogGeneticGeneration, Log, TEXT("PERSISTENCE: Existing Manager found. Re-initializing for new world."));
        
		// Just update the world and restart logic, preserving member variables
		ActiveManager->OnLevelReload(World);
	}
}

void FGeneticGenerationModule::RunMutationDemonstration(UWorld* World)
{
	UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGeneration: World Init Detected."));

	UE_LOG(LogGeneticGeneration, Log, TEXT("--- STARTING WORKFLOW TEST ---"));

    // 1. Instantiate the Wrapper
    UCustomBehaviourTree* Wrapper = NewObject<UCustomBehaviourTree>();
    if (!Wrapper) return;

    // --- STEP 1: LOAD ORIGINAL ---
    const FString PathOriginal = TEXT("/Game/Actors/EnemyUnleashed/BT_EnemyUnleashed.BT_EnemyUnleashed");
    if (!Wrapper->LoadBehaviorTree(PathOriginal))
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("[Step 1] Failed to load Original: %s"), *PathOriginal);
        return;
    }
    UE_LOG(LogGeneticGeneration, Log, TEXT("[Step 1] Loaded Original."));

    // --- STEP 2: MUTATE (Original) ---
    // This modifies the object currently in memory (The Original)
    if (!Wrapper->MutateTree_Dynamic())
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("[Step 2] Failed to mutate Original."));
        return;
    }
    UE_LOG(LogGeneticGeneration, Log, TEXT("[Step 2] Mutated Original (In-Memory)."));
	Wrapper->DebugLogTree();

    // --- STEP 3: SAVE (Generation 1) ---
    // This saves the current state (Mutated Original) to a NEW file: BT_Gen1
    const FString PathGen1 = TEXT("/Game/BehaviourTrees/Generated/BT_Gen1");
    if (!Wrapper->SaveAsNewAsset(PathGen1, true))
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("[Step 3] Failed to save Gen1 to: %s"), *PathGen1);
        return;
    }
    UE_LOG(LogGeneticGeneration, Log, TEXT("[Step 3] Saved Gen1."));

    // --- STEP 4: LOAD (Generation 1) ---
    // Crucial Test: Can we load the asset we just created?
    // This switches the Wrapper's internal pointer from 'Original' to 'BT_Gen1'
    if (!Wrapper->LoadBehaviorTree(PathGen1))
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("[Step 4] Failed to load Gen1 immediately after save."));
        return;
    }
    UE_LOG(LogGeneticGeneration, Log, TEXT("[Step 4] Verified & Loaded Gen1."));

    // --- STEP 5: MUTATE (Generation 1 -> Generation 2) ---
    // Now we are mutating 'BT_Gen1' in memory
    if (!Wrapper->MutateTree_Dynamic())
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("[Step 5] Failed to mutate Gen1."));
        return;
    }
    UE_LOG(LogGeneticGeneration, Log, TEXT("[Step 5] Mutated Gen1 (In-Memory)."));
	Wrapper->DebugLogTree();

    // --- STEP 6: SAVE (Generation 2) ---
    // Saves the double-mutated tree to BT_Gen2
    const FString PathGen2 = TEXT("/Game/BehaviourTrees/Generated/BT_Gen2");
    if (!Wrapper->SaveAsNewAsset(PathGen2, true))
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("[Step 6] Failed to save Gen2 to: %s"), *PathGen2);
        return;
    }
    UE_LOG(LogGeneticGeneration, Log, TEXT("[Step 6] Saved Gen2."));

    // --- STEP 7: LOAD (Generation 2) ---
    // Final verification
    if (!Wrapper->LoadBehaviorTree(PathGen2))
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("[Step 7] Failed to load Gen2."));
        return;
    }
    UE_LOG(LogGeneticGeneration, Log, TEXT("[Step 7] SUCCESS: Gen2 Loaded and Verified."));
    // OPTIONAL: Build Editor Graphs for visualization
    // If we don't do this, the trees work in-game but look empty in the editor
    if (FModuleManager::Get().IsModuleLoaded("BehaviourTreeGraph"))
    {
        auto& GraphModule = FModuleManager::LoadModuleChecked<FBehaviourTreeGraphModule>("BehaviourTreeGraph");
        GraphModule.BuildGraphForBehaviorTree(PathGen1);
        GraphModule.BuildGraphForBehaviorTree(PathGen2);
        UE_LOG(LogGeneticGeneration, Log, TEXT("[Visuals] Built editor graphs for Gen1 and Gen2."));
    }

    UE_LOG(LogGeneticGeneration, Log, TEXT("--- WORKFLOW TEST COMPLETE ---"));
}

void FGeneticGenerationModule::ShutdownModule()
{
	if (!IsEngineExitRequested() && ActiveManager)
	{
		UE_LOG(LogGeneticGeneration, Warning, TEXT("DEBUG: Module unloading (Hot Reload). Removing Manager from Root."));
		ActiveManager->RemoveFromRoot();
	}
    
	ActiveManager = nullptr;
	UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGenerationModule shut down."));
}
