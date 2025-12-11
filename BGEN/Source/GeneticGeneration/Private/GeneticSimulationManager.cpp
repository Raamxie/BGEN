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
#include "GameFramework/GameUserSettings.h"

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

    // Determinism Settings
	if (GEngine)
	{
		// 1. Disable Frame Smoothing (It interferes with fixed steps)
		GEngine->bSmoothFrameRate = false;

		// 2. Force a Fixed Step (e.g., 60 FPS = 0.0166s per tick)
		// This ensures the AI logic and Physics always see the exact same DeltaTime.
		GEngine->bUseFixedFrameRate = true;
		GEngine->FixedFrameRate = 60.0f; 
        
		// 3. Enable Benchmarking Mode
		// This decouples GameTime from RealTime. The engine will now run as fast 
		// as the CPU allows. If your PC can handle 600 frames per second, 
		// the simulation will run at 10x speed (600 / 60) automatically.
		FApp::SetBenchmarking(true); 
		FApp::SetFixedDeltaTime(1.0f / 60.0f);
        
		// 4. Disable VSync (Essential for uncapped speed)
		// Note: You usually set this via console command, but we can try to force it here
		// or ensure it is set in your GameUserSettings.ini
		UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
		Settings->SetVSyncEnabled(false);
		Settings->ApplySettings(true);
	}
    
    // Initial Seed
    FMath::RandInit(1337);
}

void UGeneticSimulationManager::OnLevelReload(UWorld* NewWorld)
{
    UE_LOG(LogGeneticGeneration, Display, TEXT("PERSISTENCE: Manager surviving into Generation %d"), GenerationCount);

    // 1. Update the World Context
    TargetWorld = NewWorld;

    // 2. Safety: Clear any old timers
    TimerHandle.Invalidate();
    WarmupTimerHandle.Invalidate();

    // 3. Reset Transient State
    ActiveAgents.Empty();
    
    // 4. Force Deterministic Seed for this run
    FMath::RandInit(1337);

    // 5. Restart the Logic
    SetPause(true);
    PreparePlayer();
    SpawnEnemies(1); 
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
        // Default spawn at 0,0,200 if no PlayerStart found
        FVector SpawnLoc = PlayerStart ? PlayerStart->GetActorLocation() : FVector(0, 0, 200);
        FRotator SpawnRot = PlayerStart ? PlayerStart->GetActorRotation() : FRotator::ZeroRotator;

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        APawn* SpawnedPawn = TargetWorld->SpawnActor<APawn>(PlayerPawnClass, SpawnLoc, SpawnRot, Params);
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
        // [FIX] Offset Spawn by 800 units on X. 
        // This ensures the enemy is not overlapping the player (who is at 0,0)
        // Previous value (i * 150) placed the first agent at 0,0, causing a physics explosion.
        FVector Loc = (EnemySpawnPositions.IsValidIndex(i)) 
            ? EnemySpawnPositions[i] 
            : FVector(800.0f + (i * 150.0f), 0.0f, 210.0f);
        
        ACharacter* Enemy = TargetWorld->SpawnActor<ACharacter>(EnemyClass, Loc, FRotator::ZeroRotator);
        if (!Enemy) continue;

        // 3. Inject Fitness Tracker
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
            }
            
            // 6. Track this Agent
            ActiveAgents.Add(Enemy, GenHelper);
            UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Agent %d generated. Tree: %s"), i, *FinalTree->GetName());
        }
    }
}

void UGeneticSimulationManager::SetSpawnPositions(TArray<FVector> positions)
{
    EnemySpawnPositions = positions;
}

void UGeneticSimulationManager::Simulate()
{
    UE_LOG(LogGeneticGeneration, Display, TEXT("--------------------------------------------------"));
    UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Simulate() called. Starting Warmup Phase..."));

    if (!TargetWorld) return;

    // 1. Unpause World: Allows Physics (Gravity) and Navigation to initialize
    SetPause(false); 

    // 2. [FIX] Explicitly Freeze All Agents
    // This prevents them from moving/attacking while the engine is still stuttering/loading
    for (auto& Pair : ActiveAgents)
    {
       APawn* Agent = Pair.Key;
       if (IsValid(Agent))
       {
          ACustomAIController* AI = Cast<ACustomAIController>(Agent->GetController());
          if (AI)
          {
             AI->StopMovement(); // Kill any spawn velocity
          }
       }
    }

    // 3. Start Warmup Timer
    // 2.0 seconds allows the player to fall to the floor and the NavMesh to fully build.
    float WarmupTime = 2.0f; 
    TargetWorld->GetTimerManager().SetTimer(
       WarmupTimerHandle, 
       this, 
       &UGeneticSimulationManager::OnWarmupFinished, 
       WarmupTime, 
       false
    );

    UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Warmup started. Simulation goes LIVE in %.2f seconds."), WarmupTime);
}

void UGeneticSimulationManager::OnWarmupFinished()
{
    UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Warmup Complete. STARTING AI & FITNESS TRACKING NOW."));

    // 2. Synchronized Start: Activate AI Brains
    for (auto& Pair : ActiveAgents)
    {
       APawn* Agent = Pair.Key;
       if (IsValid(Agent))
       {
          ACustomAIController* AI = Cast<ACustomAIController>(Agent->GetController());
          if (AI)
          {
             // NOW we run the tree. The world is stable.
             AI->RunAssignedTree(); 
          }
       }
    }

    // 3. Synchronized Start: Activate Fitness Trackers
    for (auto& Pair : ActiveAgents)
    {
       APawn* Agent = Pair.Key;
       if (IsValid(Agent))
       {
          UGeneticFitnessTracker* Tracker = Agent->FindComponentByClass<UGeneticFitnessTracker>();
          if (Tracker)
          {
             // BeginTracking resets the start time to NOW, ignoring the 2s warmup duration
             Tracker->BeginTracking(); 
          }
       }
    }

    // 4. Start the actual Simulation Limit Timer (e.g. 30s)
    float TimeoutGameSeconds = 30.0f;
    TargetWorld->GetTimerManager().SetTimer(
       TimerHandle, 
       this, 
       &UGeneticSimulationManager::TimerCallback, 
       TimeoutGameSeconds, 
       false
    );
}

void UGeneticSimulationManager::PlayerDiedListener()
{
    // [FIX] Ignore death if we are still warming up.
    // If the player dies here, it's a physics glitch (spawn collision), not AI skill.
    if (TargetWorld && TargetWorld->GetTimerManager().IsTimerActive(WarmupTimerHandle))
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("CRITICAL: Player died during Warmup! Likely Spawn Collision or Fall. Restarting without scoring."));
        TriggerRestart(); 
        return;
    }

    UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: EVENT: Player Died! Goal condition met."));
    StopSimulation(); 
}

void UGeneticSimulationManager::TimerCallback()
{
    UE_LOG(LogGeneticGeneration, Warning, TEXT("DEBUG: TIMEOUT! Simulation time limit reached."));
    StopSimulation(); 
}

void UGeneticSimulationManager::StopSimulation()
{
    UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: StopSimulation called. Collecting Fitness & Cleaning up..."));

    // 1. Clear ALL timers to prevent overlap
    if (TargetWorld)
    {
        TargetWorld->GetTimerManager().ClearTimer(TimerHandle);
        TargetWorld->GetTimerManager().ClearTimer(WarmupTimerHandle);
    }

    SetPause(true);

    TArray<FSimulationResult> GenerationResults;

    // 2. Gather Results
    for (auto& Pair : ActiveAgents)
    {
        APawn* Agent = Pair.Key;
        UCustomBehaviourTree* TreeWrapper = Pair.Value;

        if (IsValid(Agent) && TreeWrapper)
        {
            float FitnessScore = 0.0f;
            UGeneticFitnessTracker* Tracker = Agent->FindComponentByClass<UGeneticFitnessTracker>();
            
            if (Tracker)
            {
                FitnessScore = Tracker->CalculateFitness();
            }
            else
            {
                UE_LOG(LogGeneticGeneration, Warning, TEXT("Agent %s had no FitnessTracker! Defaulting to 0."), *Agent->GetName());
            }

            UE_LOG(LogGeneticGeneration, Log, TEXT("RESULTS: Agent [%s] | Fitness: %f"), *Agent->GetName(), FitnessScore);

            FSimulationResult Result;
            Result.BehaviorTree = TreeWrapper->GetBTAsset(); 
            Result.Fitness = FitnessScore;
            
            GenerationResults.Add(Result);
        }

        // 3. Cleanup Agents
        if (IsValid(Agent))
        {
            if (AController* C = Agent->GetController())
            {
                C->Destroy();
            }
            Agent->Destroy();
        }
    }

    ActiveAgents.Empty();
    
    UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Collected %d results. Passing to Evolution Pipeline..."), GenerationResults.Num());
    
    TriggerRestart();
}

void UGeneticSimulationManager::TriggerRestart()
{
    if (!TargetWorld) return;

    UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Restarting Level..."));

    // Double check timers are gone
    TargetWorld->GetTimerManager().ClearTimer(TimerHandle);
    TargetWorld->GetTimerManager().ClearTimer(WarmupTimerHandle);

    FString CurrentMapName = UGameplayStatics::GetCurrentLevelName(TargetWorld);
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