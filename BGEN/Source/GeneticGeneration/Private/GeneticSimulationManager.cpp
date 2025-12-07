#include "GeneticSimulationManager.h"
#include "GeneticGenerationModule.h" // For LogGeneticGeneration
#include "CustomAIController.h"
#include "CustomBehaviourTree.h"
#include "GeneticFitnessTracker.h"
#include "PlayerUnleashedBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "TimerManager.h"
#include "BehaviorTree/BlackboardData.h"

UGeneticSimulationManager::UGeneticSimulationManager()
{
    UE_LOG(LogGeneticGeneration, Log, TEXT("DEBUG: UGeneticSimulationManager constructed (Address: %p)"), this);
}

void UGeneticSimulationManager::Init(UWorld* InWorld)
{
    TargetWorld = InWorld;
    if (TargetWorld)
    {
        UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Manager Initialized. Target World: %s"), *TargetWorld->GetName());
    }
    else
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("DEBUG: Manager Init FAILED. TargetWorld is NULL!"));
    }
	if (GEngine)
	{
		GEngine->bSmoothFrameRate = false;
		GEngine->FixedFrameRate = 0.0f; // Uncapped
	}
}

void UGeneticSimulationManager::OnLevelReload(UWorld* NewWorld)
{
	UE_LOG(LogGeneticGeneration, Display, TEXT("PERSISTENCE: Manager surviving into Generation %d"), GenerationCount);

	// 1. Update the World Context
	TargetWorld = NewWorld;

	// 2. Safety: Clear any old timers from the previous world
	// (Although the world destruction usually handles this, it's safe to reset the handle)
	TimerHandle.Invalidate();

	// 3. Reset Transient State
	EnemySpawnPositions.Empty(); 
	// Clear any lists of actors from the old world, as those pointers are now invalid garbage!
    
	// 4. Restart the Logic
	SetPause(true);
	PreparePlayer();
	SpawnEnemies(1); // Pass gen count or difficulty based on 'GenerationCount'
	Simulate();
}

UWorld* UGeneticSimulationManager::GetWorld() const
{
    return TargetWorld;
}

void UGeneticSimulationManager::PreparePlayer()
{
    UE_LOG(LogGeneticGeneration, Log, TEXT("DEBUG: PreparePlayer called."));

    if (!TargetWorld) return;

    APlayerUnleashedBase* PlayerUnleashed = nullptr;

    // 1. Try to find existing first
    AActor* ExistingActor = UGameplayStatics::GetPlayerCharacter(TargetWorld, 0);
    PlayerUnleashed = Cast<APlayerUnleashedBase>(ExistingActor);

    // 2. If not found, spawn it
    if (!PlayerUnleashed)	
    {
        UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Player not found or wrong class. Spawning..."));

        UClass* PlayerPawnClass = GetClassFromPath("/Game/Actors/PlayerUnleashed/PlayerUnleashed.PlayerUnleashed_C");
        if (!PlayerPawnClass) return;

        AActor* PlayerStart = UGameplayStatics::GetActorOfClass(TargetWorld, APlayerStart::StaticClass());
        FVector SpawnLoc = PlayerStart ? PlayerStart->GetActorLocation() : FVector(0, 0, 200);
        FRotator SpawnRot = PlayerStart ? PlayerStart->GetActorRotation() : FRotator::ZeroRotator;

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        // SPAWN
        APawn* SpawnedPawn = TargetWorld->SpawnActor<APawn>(PlayerPawnClass, SpawnLoc, SpawnRot, Params);
        
        // CAST IMMEDIATELY
        PlayerUnleashed = Cast<APlayerUnleashedBase>(SpawnedPawn);

        if (SpawnedPawn && TargetWorld->GetFirstPlayerController())
        {
            TargetWorld->GetFirstPlayerController()->Possess(SpawnedPawn);
        }
    }

    // 3. Bind Event
    if (PlayerUnleashed)
    {
        PlayerUnleashed->OnPlayerEvent.RemoveAll(this);
        PlayerUnleashed->OnPlayerEvent.AddDynamic(this, &UGeneticSimulationManager::PlayerDiedListener);
        UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: SUCCESS - Bound to Player Death Delegate."));
    }
    else
    {
        // If this prints, the Reparenting (Step 1) failed.
        UE_LOG(LogGeneticGeneration, Error, TEXT("DEBUG: FAILURE - Spawned actor is NOT a APlayerUnleashedBase. Did you reparent the Blueprint?"));
    }
}

void UGeneticSimulationManager::SpawnEnemies(int32 AmountToSpawn)
{
    UE_LOG(LogGeneticGeneration, Log, TEXT("DEBUG: SpawnEnemies called. Requesting %d enemies."), AmountToSpawn);

    if (!TargetWorld) return;

    // 1. Load Assets
    UClass* EnemyClass = GetClassFromPath("/Game/Actors/EnemyUnleashed/EnemyUnleashed.EnemyUnleashed_C");
    UClass* ControllerClass = GetClassFromPath("/Game/Actors/EnemyUnleashed/AIController_EnemyUnleashed.AIController_EnemyUnleashed_C");
    UBlackboardData* BBAsset = Cast<UBlackboardData>(LoadObjectFromPath("/Game/Actors/EnemyUnleashed/BB_EnemyUnleashed.BB_EnemyUnleashed"));

    if (!EnemyClass || !ControllerClass || !BBAsset)
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("DEBUG: Critical Asset missing. Aborting Spawn."));
        return;
    }

    // 2. Spawn Loop
    for (int32 i = 0; i < AmountToSpawn; i++)
    {
        // Simple grid offset for spawn location to avoid collision
        FVector Loc = (EnemySpawnPositions.IsValidIndex(i)) ? EnemySpawnPositions[i] : FVector(i * 150.0f, 0.0f, 210.0f);
        
        ACharacter* Enemy = TargetWorld->SpawnActor<ACharacter>(EnemyClass, Loc, FRotator::ZeroRotator);
        if (!Enemy) continue;

        // 3. Inject Fitness Tracker (But do NOT start it yet)
        UGeneticFitnessTracker* FitnessComp = NewObject<UGeneticFitnessTracker>(Enemy);
        if (FitnessComp)
        {
            FitnessComp->RegisterComponent(); 
            Enemy->AddInstanceComponent(FitnessComp);
        }

        // 4. Spawn & Possess AI
        ACustomAIController* AI = TargetWorld->SpawnActor<ACustomAIController>(ControllerClass, Loc, FRotator::ZeroRotator);
        if (AI)
        {
            AI->Possess(Enemy);
            
            // 5. Generate & Assign Tree
            UCustomBehaviourTree* GenHelper = NewObject<UCustomBehaviourTree>(this);
            GenHelper->LoadBehaviorTree("/Game/Actors/EnemyUnleashed/BT_EnemyUnleashed.BT_EnemyUnleashed");
            
            // TODO: Apply Mutations here using GenHelper->MutateTree(...)

            UBehaviorTree* FinalTree = GenHelper->GetBTAsset();
            if (FinalTree)
            {
                AI->AssignTree(FinalTree, BBAsset);
                AI->RunAssignedTree();
            }
            
            // 6. Track this Agent
            ActiveAgents.Add(Enemy, GenHelper);
            UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Agent %d generated. Tree: %s"), i, *FinalTree->GetName());
        }
    }
}

void UGeneticSimulationManager::SetSpawnPositions(TArray<FVector> positions)
{
}

void UGeneticSimulationManager::Simulate()
{
	UE_LOG(LogGeneticGeneration, Display, TEXT("--------------------------------------------------"));
	UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Simulate() called. Initializing Run..."));

	if (!TargetWorld)
	{
		UE_LOG(LogGeneticGeneration, Error, TEXT("DEBUG: TargetWorld is invalid! Cannot simulate."));
		return;
	}

	// 1. Verify we actually have agents to simulate
	if (ActiveAgents.Num() == 0)
	{
		UE_LOG(LogGeneticGeneration, Warning, TEXT("DEBUG: No Active Agents found! Did SpawnEnemies fail?"));
		StopSimulation(); // Immediate exit
		return;
	}

	// 2. Synchronized Start: Activate all Fitness Trackers NOW
	int32 TrackersStarted = 0;
	for (auto& Pair : ActiveAgents)
	{
		APawn* Agent = Pair.Key;
		if (IsValid(Agent))
		{
			UGeneticFitnessTracker* Tracker = Agent->FindComponentByClass<UGeneticFitnessTracker>();
			if (Tracker)
			{
				Tracker->BeginTracking(); // Start the stopwatch here!
				TrackersStarted++;
			}
		}
	}
	UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Fitness Tracking started for %d agents."), TrackersStarted);

	// 3. Set Global Time Dilation (Optional speed up)
	UGameplayStatics::SetGlobalTimeDilation(TargetWorld, 1.0f); // Set to 5.0f to train faster!

	// 4. Set Safety Timer (Simulation Duration)
	float TimeoutGameSeconds = 30.0f;
	TargetWorld->GetTimerManager().SetTimer(
	   TimerHandle, 
	   this, 
	   &UGeneticSimulationManager::TimerCallback, 
	   TimeoutGameSeconds, 
	   false
	);
    
	// 5. GO!
	SetPause(false);
    
	UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Simulation LIVE. Max Duration: %.2fs"), TimeoutGameSeconds);
	UE_LOG(LogGeneticGeneration, Display, TEXT("--------------------------------------------------"));
}

void UGeneticSimulationManager::TimerCallback()
{
	UE_LOG(LogGeneticGeneration, Warning, TEXT("DEBUG: TIMEOUT! Simulation time limit reached."));
	StopSimulation(); 
}

void UGeneticSimulationManager::PlayerDiedListener()
{
	UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: EVENT: Player Died! Goal condition met."));
	StopSimulation(); 
}

void UGeneticSimulationManager::StopSimulation()
{
    UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: StopSimulation called. Collecting Fitness & Cleaning up..."));

    // 1. Clear any pending timer so we don't timeout while processing results
    if (TargetWorld)
    {
        TargetWorld->GetTimerManager().ClearTimer(TimerHandle);
    }

    SetPause(true);

    // 2. Container for this generation's results
    TArray<FSimulationResult> GenerationResults;

    // 3. Iterate over all active agents to gather fitness
    for (auto& Pair : ActiveAgents)
    {
        APawn* Agent = Pair.Key;
        UCustomBehaviourTree* TreeWrapper = Pair.Value;

        // Ensure agent and tree are still valid
        if (IsValid(Agent) && TreeWrapper)
        {
            float FitnessScore = 0.0f;

            // Retrieve the Fitness Component we attached in SpawnEnemies
            UGeneticFitnessTracker* Tracker = Agent->FindComponentByClass<UGeneticFitnessTracker>();
            if (Tracker)
            {
                FitnessScore = Tracker->CalculateFitness();
            }
            else
            {
                UE_LOG(LogGeneticGeneration, Warning, TEXT("Agent %s had no FitnessTracker! Defaulting to 0."), *Agent->GetName());
            }

            // Log for debug transparency
            UE_LOG(LogGeneticGeneration, Log, TEXT("RESULTS: Agent [%s] | Fitness: %f"), *Agent->GetName(), FitnessScore);

            // Store result
            FSimulationResult Result;
            Result.BehaviorTree = TreeWrapper->GetBTAsset(); // The actual UBehaviorTree*
            Result.Fitness = FitnessScore;
            
            GenerationResults.Add(Result);
        }

        // 4. Cleanup: Destroy the Agent and Controller explicitly
        if (IsValid(Agent))
        {
            if (AController* C = Agent->GetController())
            {
                C->Destroy();
            }
            Agent->Destroy();
        }
    }

    // 5. Clear the tracking map
    ActiveAgents.Empty();

    // 6. CRITICAL: Pass results to your Genetic Module or Store them
    // WARNING: 'TriggerRestart' calls OpenLevel, which will DESTROY this Manager instance 
    // and lose 'GenerationResults' unless you pass them to a persistent object (like GameInstance or your Module).
    UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Collected %d results. Passing to Evolution Pipeline..."), GenerationResults.Num());
    
    // Example hook: 
    // FGeneticGenerationModule::Get().ProcessGenerationResults(GenerationResults);

    // 7. Restart the loop
    TriggerRestart();
}

void UGeneticSimulationManager::TriggerRestart()
{
	if (!TargetWorld) return;

	UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Restarting Level..."));

	// Ensure the timer is cleared so it doesn't fire while the map is unloading
	if (TargetWorld->GetTimerManager().TimerExists(TimerHandle))
	{
		TargetWorld->GetTimerManager().ClearTimer(TimerHandle);
	}

	// Get current map name (e.g., "Main")
	FString CurrentMapName = UGameplayStatics::GetCurrentLevelName(TargetWorld);

	// Reload the map
	UGameplayStatics::OpenLevel(TargetWorld, FName(*CurrentMapName));
}

void UGeneticSimulationManager::SetPause(bool bPauseState)
{
    if (TargetWorld)
    {
        UGameplayStatics::SetGamePaused(TargetWorld, bPauseState);
        UE_LOG(LogGeneticGeneration, Log, TEXT("DEBUG: World Paused state set to: %s"), bPauseState ? TEXT("TRUE") : TEXT("FALSE"));
    }
}

bool UGeneticSimulationManager::DoesPlayerExist() const
{
    return UGameplayStatics::GetPlayerCharacter(TargetWorld, 0) != nullptr;
}

UClass* UGeneticSimulationManager::GetClassFromPath(const FString& Path)
{
    UClass* Cls = StaticLoadClass(UObject::StaticClass(), nullptr, *Path);
    if (!Cls) UE_LOG(LogGeneticGeneration, Warning, TEXT("DEBUG: Failed to load class: %s"), *Path);
    return Cls;
}

UObject* UGeneticSimulationManager::LoadObjectFromPath(const FString& Path)
{
    UObject* Obj = StaticLoadObject(UObject::StaticClass(), nullptr, *Path);
    if (!Obj) UE_LOG(LogGeneticGeneration, Warning, TEXT("DEBUG: Failed to load object: %s"), *Path);
    return Obj;
}

bool UGeneticSimulationManager::IsPaused() const
{
    return UGameplayStatics::IsGamePaused(TargetWorld);
}