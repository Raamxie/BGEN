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
    if (!FParse::Param(FCommandLine::Get(), TEXT("GeneticRun")))
    {
        UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGeneration module skipped (no -GeneticRun flag)."));
        return;
    }

    UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGenerationModule started."));    

    static FOutputDeviceFile GeneticLogFile(*FPaths::Combine(FPaths::ProjectLogDir(), TEXT("GeneticGeneration.log")), true);
    if (!GLog->IsRedirectingTo(&GeneticLogFile))
    {
        GLog->AddOutputDevice(&GeneticLogFile);
    }

    ActiveManager = NewObject<UGeneticSimulationManager>(GetTransientPackage());
    ActiveManager->AddToRoot(); 
    
    ActiveManager->OnEpochComplete.AddRaw(this, &FGeneticGenerationModule::OnEpochFinished);
    FWorldDelegates::OnPostWorldInitialization.AddRaw(this, &FGeneticGenerationModule::OnWorldInitialized);
    
    // Bind End Frame to allow explicit yielding
    FCoreDelegates::OnEndFrame.AddRaw(this, &FGeneticGenerationModule::OnEndFrame);

    CurrentEpoch = 0;
    TotalEpochs = 1000; 
    bIsRunningGeneticLoop = true;
    bIsWaitingForJob = false;

    if (GEngine && GEngine->IsInitialized())
    {
        OnEngineInitComplete();
    }
    else
    {
        FCoreDelegates::OnPostEngineInit.AddRaw(this, &FGeneticGenerationModule::OnEngineInitComplete);
    }
}

// =========================================================================
// CRITICAL FIX: Restored the missing Macro
IMPLEMENT_MODULE(FGeneticGenerationModule, GeneticGeneration)
// =========================================================================

void FGeneticGenerationModule::OnEndFrame()
{
	// Force the Game Thread to yield to the OS when idle
	if (bIsWaitingForJob)
	{
		FPlatformProcess::Sleep(0.1f);
	}
}

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
        
		bIsWaitingForJob = true; // Start sleeping

		if (UGameInstance* GI = world->GetGameInstance())
		{
			if (UWorkerNetworkSubsystem* NetSub = GI->GetSubsystem<UWorkerNetworkSubsystem>())
			{
				if (ActiveManager) ActiveManager->Init(world);

				NetSub->OnJobReceived.RemoveAll(ActiveManager);
				NetSub->OnJobReceived.AddDynamic(ActiveManager, &UGeneticSimulationManager::TransitionToMainMap);
				
				NetSub->RequestJobFromMaster();
			}
		}
	}
	else if (MapName.Contains(TEXT("Main")))
	{
		bIsWaitingForJob = false; // Stop sleeping immediately

		UE_LOG(LogGeneticGeneration, Log, TEXT("Entered Main Map. Starting Simulation Instantly..."));
		RunSimulation(world);
	}
}

void FGeneticGenerationModule::RunSimulation(UWorld* World)
{
	if (!ActiveManager) return;

	ActiveManager->Init(World);

	if (UGameInstance* GameInstance = World->GetGameInstance())
	{
		if (UWorkerNetworkSubsystem* NetSubsystem = GameInstance->GetSubsystem<UWorkerNetworkSubsystem>())
		{
			if (!NetSubsystem->CurrentJobAssetPath.IsEmpty())
			{
				UE_LOG(LogGeneticGeneration, Warning, TEXT("WORKER: Spawning assigned AI: %s"), *NetSubsystem->CurrentJobAssetPath);
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
		LoadEmptyWaitingMap();
	}
}

void FGeneticGenerationModule::ShutdownModule()
{
	FCoreDelegates::OnEndFrame.RemoveAll(this); 

    if (ActiveManager)
    {
        ActiveManager->RemoveFromRoot();
        ActiveManager = nullptr;
    }
    
    UE_LOG(LogGeneticGeneration, Log, TEXT("GeneticGenerationModule shut down."));
}