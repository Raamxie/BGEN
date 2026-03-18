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
#include "WorkerNetworkSubsystem.h"
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
    TotalEpochs = 1000; // Set desired amount
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
	UE_LOG(LogGeneticGeneration, Log, TEXT("Engine Init Complete. Loading EmptyWaiting Map..."));
	LoadEmptyWaitingMap();
}

void FGeneticGenerationModule::LoadEmptyWaitingMap()
{
	if (!bIsRunningGeneticLoop) return;

	UWorld* WorldContext = nullptr;
	if (GEngine && GEngine->GetWorldContexts().Num() > 0)
	{
		WorldContext = GEngine->GetWorldContexts()[0].World();
	}

	if (WorldContext)
	{
		UE_LOG(LogGeneticGeneration, Log, TEXT("Loading EmptyWaiting Map (Epoch %d)"), CurrentEpoch);
		UGameplayStatics::OpenLevel(WorldContext, FName("EmptyWaiting"));
	}
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
		UE_LOG(LogGeneticGeneration, Log, TEXT("Loading Main Simulation Map (Epoch %d)"), CurrentEpoch);
		UGameplayStatics::OpenLevel(WorldContext, FName("Main"));
	}
}

void FGeneticGenerationModule::OnWorldInitialized(UWorld* world, const UWorld::InitializationValues IVS)
{
	if (!world || !world->IsGameWorld()) return;

	FString MapName = world->GetName();

	if (MapName.Contains(TEXT("EmptyWaiting")))
	{
		UE_LOG(LogGeneticGeneration, Log, TEXT("Entered EmptyWaiting Map. Requesting Job from Server..."));
        
		if (GEngine) GEngine->SetMaxFPS(5.0f);

		if (UGameInstance* GI = world->GetGameInstance())
		{
			if (UWorkerNetworkSubsystem* NetSub = GI->GetSubsystem<UWorkerNetworkSubsystem>())
			{
				// 1. Give Manager the current world context
				if (ActiveManager) ActiveManager->Init(world);

				// 2. Bind the manager to transition the map the moment the job arrives
				NetSub->OnJobReceived.RemoveAll(ActiveManager);
				NetSub->OnJobReceived.AddDynamic(ActiveManager, &UGeneticSimulationManager::TransitionToMainMap);
				
				// 3. Ask the server
				NetSub->RequestJobFromMaster();
			}
		}
	}
	else if (MapName.Contains(TEXT("Main")))
	{
		if (GEngine) GEngine->SetMaxFPS(0.0f); 

		UE_LOG(LogGeneticGeneration, Log, TEXT("Entered Main Map. Starting Simulation Instantly..."));
		RunSimulation(world);
	}
}

void FGeneticGenerationModule::RunSimulation(UWorld* World)
{
	if (!ActiveManager) return;

	// 1. Hand over the new Main World Context
	ActiveManager->Init(World);

	// 2. Grab the job we downloaded while in the waiting room
	if (UGameInstance* GameInstance = World->GetGameInstance())
	{
		if (UWorkerNetworkSubsystem* NetSubsystem = GameInstance->GetSubsystem<UWorkerNetworkSubsystem>())
		{
			if (!NetSubsystem->CurrentJobAssetPath.IsEmpty())
			{
				UE_LOG(LogGeneticGeneration, Warning, TEXT("WORKER: Spawning assigned AI: %s"), *NetSubsystem->CurrentJobAssetPath);
				
				// 3. Start immediately! No network delay, no imposter AI.
				ActiveManager->StartEpochWithJob(NetSubsystem->CurrentJobAssetPath);
			}
			else
			{
				UE_LOG(LogGeneticGeneration, Error, TEXT("WORKER: Loaded Main map but had no job! Retreating..."));
				LoadEmptyWaitingMap();
			}
		}
	}
}

void FGeneticGenerationModule::OnEpochFinished()
{
	UE_LOG(LogGeneticGeneration, Log, TEXT("--- EPOCH %d COMPLETE ---"), CurrentEpoch);
	CurrentEpoch++;
    
	if (ActiveManager) ActiveManager->GenerationCount = CurrentEpoch + 1;

	if (CurrentEpoch >= TotalEpochs)
	{
		bIsRunningGeneticLoop = false;
	}
	else
	{
		// Epoch finished -> Go back to the waiting room to wipe memory and rest
		LoadEmptyWaitingMap();
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