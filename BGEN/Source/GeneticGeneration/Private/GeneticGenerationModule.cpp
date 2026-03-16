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

	// Use .Contains() to bypass the "UEDPIE_0_" prefix added by the Editor
	if (MapName.Contains(TEXT("EmptyWaiting")))
	{
		UE_LOG(LogGeneticGeneration, Log, TEXT("Entered EmptyWaiting Map. Requesting Job..."));
        
		// Save CPU by dropping FPS. No need to freeze the world, the empty map takes 0 resources!
		if (GEngine) GEngine->SetMaxFPS(5.0f);

		if (UGameInstance* GI = world->GetGameInstance())
		{
			if (UWorkerNetworkSubsystem* NetSub = GI->GetSubsystem<UWorkerNetworkSubsystem>())
			{
				NetSub->RequestJobFromMaster();
			}
		}
	}
	else if (MapName.Contains(TEXT("Main")))
	{
		// Restore FPS for the simulation
		if (GEngine) GEngine->SetMaxFPS(0.0f); 

		bool bHasValidJob = false;
		if (UGameInstance* GI = world->GetGameInstance())
		{
			if (UWorkerNetworkSubsystem* NetSub = GI->GetSubsystem<UWorkerNetworkSubsystem>())
			{
				bHasValidJob = !NetSub->CurrentJobAssetPath.IsEmpty();
			}
		}

		if (bHasValidJob)
		{
			UE_LOG(LogGeneticGeneration, Log, TEXT("Entered Main Map. Starting Simulation..."));
			RunSimulation(world);
		}
		else
		{
			UE_LOG(LogGeneticGeneration, Warning, TEXT("Entered Main Map without a job! Redirecting to EmptyWaiting..."));
			LoadEmptyWaitingMap();
		}
	}
}

void FGeneticGenerationModule::RunSimulation(UWorld* World)
{
	if (!ActiveManager) return;

	UE_LOG(LogGeneticGeneration, Log, TEXT("--- WORKER: MAP LOADED, REQUESTING JOB ---"));

	// 1. Hand over the new World Context
	ActiveManager->Init(World);

	// 2. Ask the Master for a job
	if (UGameInstance* GameInstance = World->GetGameInstance())
	{
		if (UWorkerNetworkSubsystem* NetSubsystem = GameInstance->GetSubsystem<UWorkerNetworkSubsystem>())
		{
			// Unbind any old delegates to prevent double-firing after map reloads
			NetSubsystem->OnJobReceived.RemoveAll(ActiveManager);
			
			// Bind the Manager's StartEpoch function to fire when the job arrives
			NetSubsystem->OnJobReceived.AddDynamic(ActiveManager, &UGeneticSimulationManager::StartEpochWithJob);
			
			// Send the HTTP GET
			NetSubsystem->RequestJobFromMaster();
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