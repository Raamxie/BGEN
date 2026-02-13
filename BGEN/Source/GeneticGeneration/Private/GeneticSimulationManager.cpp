#include "GeneticSimulationManager.h"
#include "GeneticGenerationModule.h"
#include "CustomAIController.h"
#include "CustomBehaviourTree.h"
#include "GeneticFitnessTracker.h"
#include "PlayerUnleashedBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "TimerManager.h"
#include "BehaviorTree/BlackboardData.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "GeneticSelectionLibrary.h"
#include "GeneticMutationLibrary.h"

UGeneticSimulationManager::UGeneticSimulationManager()
{
}

void UGeneticSimulationManager::Init(UWorld* InWorld)
{
    TargetWorld = InWorld;
	
	if (GEngine)
	{
		GEngine->bSmoothFrameRate = false; 
		GEngine->bUseFixedFrameRate = true; 
		GEngine->FixedFrameRate = 60.0f;    
	}
}	

void UGeneticSimulationManager::StartEpoch()
{
	UE_LOG(LogGeneticGeneration, Display, TEXT("--------------------------------------------------"));
	UE_LOG(LogGeneticGeneration, Display, TEXT("MANAGER: Starting Generation %d"), GenerationCount);

    if (!TargetWorld)
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("MANAGER: Cannot start epoch - TargetWorld is null!"));
        return;
    }

	if (GenerationCount == 1)
	{
		FString HistoryLogPath = FPaths::ProjectLogDir() / TEXT("EvolutionHistory.txt");
        
		// Delete the file if it exists, so we start fresh for this session
		if (FPaths::FileExists(HistoryLogPath))
		{
			IFileManager::Get().Delete(*HistoryLogPath);
			UE_LOG(LogGeneticGeneration, Warning, TEXT("Manager: Cleared old Evolution History log."));
		}
	}
	
	// 1. Cleanup from previous runs (Safety)
	if (TimerHandle.IsValid())
    {
        TargetWorld->GetTimerManager().ClearTimer(TimerHandle);
    }
	EnemySpawnPositions.Empty(); 
    
	// 2. DECIDE: Pick the "Parent" Tree
	FString SeedPath = TEXT("/Game/Actors/EnemyUnleashed/Test");
	
	// 3. EXECUTE: Setup the arena
	SetPause(true);
	PreparePlayer();
	
    // 4. SPAWN: Pass the chosen gene to the spawner
	SpawnEnemies(1, SeedPath); 
	
    // 5. RUN
	Simulate();
}

FString UGeneticSimulationManager::SelectTreeToEvolve()
{
	// 1. First Run Case: No history yet
	if (AllTimeResults.Num() == 0)
	{
		UE_LOG(LogGeneticGeneration, Warning, TEXT("Selection: First run. Loading default tree."));
		return TEXT("/Game/Actors/EnemyUnleashed/Test"); 
	}

	// 2. Step 1: Filter for "Successful" Agents (Fitness > 0)
	TArray<FSimulationResult> PositiveCandidates;
	for (const FSimulationResult& Result : AllTimeResults)
	{
		if (Result.Fitness > 0.0f)
		{
			PositiveCandidates.Add(Result);
		}
	}

	// 3. Step 2: Decision
	if (PositiveCandidates.Num() > 0)
	{
		// Scenario A: We have at least one successful ancestor. Pick one randomly.
		int32 RandIndex = FMath::RandRange(0, PositiveCandidates.Num() - 1);
		const FSimulationResult& Winner = PositiveCandidates[RandIndex];
        
		UE_LOG(LogGeneticGeneration, Display, TEXT("Selection: Picked successful ancestor from Gen %d (Fitness: %.2f)"), Winner.GenerationNumber, Winner.Fitness);
		return Winner.BehaviorTreePath;
	}
	else
	{
		// Scenario B: No one has succeeded yet. Pick ANY previous attempt to mutate further.
		int32 RandIndex = FMath::RandRange(0, AllTimeResults.Num() - 1);
		const FSimulationResult& RandomPick = AllTimeResults[RandIndex];

		UE_LOG(LogGeneticGeneration, Warning, TEXT("Selection: No positive fitness found. Picking random ancestor from Gen %d."), RandomPick.GenerationNumber);
		return RandomPick.BehaviorTreePath;
	}
}


UWorld* UGeneticSimulationManager::GetWorld() const
{
    return TargetWorld;
}

void UGeneticSimulationManager::FinishEpoch()
{
	UE_LOG(LogGeneticGeneration, Log, TEXT("Manager: Epoch Finished. Notifying Module..."));
	if (OnEpochComplete.IsBound())
	{
		OnEpochComplete.Broadcast();
	}
}

void UGeneticSimulationManager::PreparePlayer()
{
	if (!TargetWorld) return;

	// Reset reference
	ActivePlayer = nullptr;

	// 1. Try to find existing
	AActor* ExistingActor = UGameplayStatics::GetPlayerCharacter(TargetWorld, 0);
	ActivePlayer = Cast<APlayerUnleashedBase>(ExistingActor);

	// 2. Spawn if missing
	if (!ActivePlayer)    
	{
		UClass* PlayerPawnClass = GetClassFromPath("/Game/Actors/PlayerUnleashed/PlayerUnleashed.PlayerUnleashed_C");
		if (PlayerPawnClass)
		{
			AActor* PlayerStart = UGameplayStatics::GetActorOfClass(TargetWorld, APlayerStart::StaticClass());
			FVector SpawnLoc = PlayerStart ? PlayerStart->GetActorLocation() : FVector(0, 0, 200);
			FRotator SpawnRot = PlayerStart ? PlayerStart->GetActorRotation() : FRotator::ZeroRotator;
            
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            
			APawn* SpawnedPawn = TargetWorld->SpawnActor<APawn>(PlayerPawnClass, SpawnLoc, SpawnRot, Params);
            
			ActivePlayer = Cast<APlayerUnleashedBase>(SpawnedPawn);

			// Try to possess (might fail if PC doesn't exist yet, but we have the Pawn reference now!)
			if (SpawnedPawn && TargetWorld->GetFirstPlayerController())
			{
				TargetWorld->GetFirstPlayerController()->Possess(SpawnedPawn);
			}
		}
	}

	// 3. Setup Listener on the Manager itself (existing logic)
	if (ActivePlayer)
	{
		ActivePlayer->OnPlayerEvent.RemoveAll(this);
		ActivePlayer->OnPlayerEvent.AddDynamic(this, &UGeneticSimulationManager::PlayerDiedListener);
	}
}

void UGeneticSimulationManager::SpawnEnemies(int32 AmountToSpawn, FString GenomePath)
{
    if (!TargetWorld) return;

    // 1. Resolve Assets
    UClass* EnemyClass = GetClassFromPath("/Game/Actors/EnemyUnleashed/EnemyUnleashed.EnemyUnleashed_C");
    UClass* ControllerClass = GetClassFromPath("/Game/Actors/EnemyUnleashed/AIController_EnemyUnleashed.AIController_EnemyUnleashed_C");
    UBlackboardData* BBAsset = Cast<UBlackboardData>(LoadObjectFromPath("/Game/Actors/EnemyUnleashed/BB_EnemyUnleashed.BB_EnemyUnleashed"));

    if (!EnemyClass || !ControllerClass || !BBAsset) 
    {
        UE_LOG(LogGeneticGeneration, Error, TEXT("SpawnEnemies: Missing Assets!"));
        return;
    }

    UE_LOG(LogGeneticGeneration, Log, TEXT("--- SPAWNING GENERATION %d (%d Agents) ---"), GenerationCount, AmountToSpawn);

    for (int32 i = 0; i < AmountToSpawn; i++)
    {
        // 2. Spawn Logic
        FVector Loc = (EnemySpawnPositions.IsValidIndex(i)) ? EnemySpawnPositions[i] : FVector(i * 150.0f, 0.0f, 210.0f);
        ACharacter* Enemy = TargetWorld->SpawnActor<ACharacter>(EnemyClass, Loc, FRotator::ZeroRotator);
        ACustomAIController* AI = TargetWorld->SpawnActor<ACustomAIController>(ControllerClass, Loc, FRotator::ZeroRotator);

        if (!Enemy || !AI) continue;

        // Attach Fitness
        UGeneticFitnessTracker* FitnessComp = NewObject<UGeneticFitnessTracker>(Enemy);
        FitnessComp->RegisterComponent();
        Enemy->AddInstanceComponent(FitnessComp);
        
        AI->Possess(Enemy);

        // 3. GENOME LOGIC
        UCustomBehaviourTree* ChildWrapper = nullptr;
        FEvolutionHistory History;
        bool bIsSeed = false;

        // A. EVOLUTION (History Exists)
        if (AllTimeResults.Num() > 0)
        {
            History.SelectionMethod = TEXT("Tournament");
            FSimulationResult ParentA = UGeneticSelectionLibrary::TournamentSelection(AllTimeResults, 3);
            FSimulationResult ParentB = UGeneticSelectionLibrary::TournamentSelection(AllTimeResults, 3);

            if (UGeneticSelectionLibrary::IsValidResult(ParentA))
            {
                UCustomBehaviourTree* WrapperA = NewObject<UCustomBehaviourTree>(this);
                WrapperA->LoadBehaviorTree(ParentA.BehaviorTreePath);
                
                // Crossover
                if (FMath::FRand() < 0.7f) 
                {
                    UCustomBehaviourTree* WrapperB = NewObject<UCustomBehaviourTree>(this);
                    if(WrapperB->LoadBehaviorTree(ParentB.BehaviorTreePath))
                    {
                         FString Log;
                         ChildWrapper = WrapperA->PerformCrossover(WrapperB, Log);
                         History.CrossoverLog = Log;
                    }
                }
                
                // Clone if Crossover didn't happen
                if (!ChildWrapper)
                {
                    ChildWrapper = NewObject<UCustomBehaviourTree>(this);
                    ChildWrapper->InitFromTreeInstance(WrapperA->GetBTAsset());
                    History.CrossoverLog = TEXT("Clone");
                }
                History.ParentA_Path = ParentA.BehaviorTreePath;
            }
        }

        // B. SEED FALLBACK (First Run or Selection Failed)
        if (!ChildWrapper)
        {
            ChildWrapper = NewObject<UCustomBehaviourTree>(this);
            if(ChildWrapper->LoadBehaviorTree(GenomePath))
            {
                bIsSeed = true;
                History.SelectionMethod = TEXT("Seed (Gen 0)");
                History.ParentA_Path = GenomePath;
            }
            else
            {
                UE_LOG(LogGeneticGeneration, Error, TEXT("Failed to load Seed: %s"), *GenomePath);
                AI->Destroy(); Enemy->Destroy(); continue;
            }
        }

        // 4. MUTATION & ASSIGNMENT
        if (ChildWrapper && ChildWrapper->GetBTAsset())
        {
            FString MLog;

            // BOOTSTRAP: If Seed (Gen 0), Force Heavy Mutation to create diversity
            if (bIsSeed)
            {
                // Mutate 5 times to generate a complex tree immediately
                for(int k=0; k<5; k++)
                {
                    MLog += UGeneticMutationLibrary::MutateTree(ChildWrapper, 1.0f) + " | ";
                }
                History.MutationLog = TEXT("BOOTSTRAP: ") + MLog;
            }
            else
            {
                // Standard Evolution
                History.MutationLog = UGeneticMutationLibrary::MutateTree(ChildWrapper, 0.40f);
            }

            // Save History & Assign
            ChildWrapper->EvolutionData = History;
            AI->AssignTree(ChildWrapper->GetBTAsset(), BBAsset);
            
            // 5. CRITICAL: Add to Tracking List
            ActiveAgents.Add(Enemy, ChildWrapper);
            
            UE_LOG(LogGeneticGeneration, Log, TEXT("Agent %d Spawned & Registered. Tree: %s"), i, *ChildWrapper->GetBTAsset()->GetName());
        }
    }
}

void UGeneticSimulationManager::Simulate()
{
	if (!TargetWorld || ActiveAgents.Num() == 0)
	{
		StopSimulation(); 
		return;
	}

	// Pass the cached ActivePlayer to every agent
	for (auto& Pair : ActiveAgents)
	{
		APawn* Agent = Pair.Key;
		if (IsValid(Agent))
		{
			if (ACustomAIController* AI = Cast<ACustomAIController>(Agent->GetController()))
			{
				AI->RunAssignedTree();
			}
			if (UGeneticFitnessTracker* Tracker = Agent->FindComponentByClass<UGeneticFitnessTracker>())
			{
				// PASS THE PLAYER HERE
				Tracker->BeginTracking(ActivePlayer);
			}
		}
	}

	// UGameplayStatics::SetGlobalTimeDilation(TargetWorld, 4.0f);
	TargetWorld->GetTimerManager().SetTimer(TimerHandle, this, &UGeneticSimulationManager::TimerCallback, 30.0f, false);
	SetPause(false);
}

// ... [Include TimerCallback, StopSimulation (With fix), PlayerDiedListener etc] ...

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
	if (TargetWorld && TimerHandle.IsValid())
	{
		TargetWorld->GetTimerManager().ClearTimer(TimerHandle);
	}
	SetPause(true);

	UE_LOG(LogGeneticGeneration, Log, TEXT("Manager: Stopping Simulation. Processing %d Agents..."), ActiveAgents.Num());

	for (auto& Pair : ActiveAgents)
	{
		APawn* Agent = Pair.Key;
		UCustomBehaviourTree* TreeWrapper = Pair.Value;

		if (IsValid(Agent) && TreeWrapper)
		{
			// 1. Calculate Fitness
			float FitnessScore = 0.0f;
			if (UGeneticFitnessTracker* Tracker = Agent->FindComponentByClass<UGeneticFitnessTracker>())
			{
				FitnessScore = Tracker->CalculateFitness();
			}

			// 2. Save Asset for next generation
			FString SaveName = FString::Printf(TEXT("/Game/BehaviourTrees/Generated/Gen%d_Agent_%d"), GenerationCount, 1);
			FString FinalAssetPath = TreeWrapper->SaveAsNewAsset(SaveName, true);
            
			// 3. Record Result
			FSimulationResult RunResult;
			RunResult.BehaviorTreePath = FinalAssetPath;
			RunResult.Fitness = FitnessScore;
			RunResult.GenerationNumber = GenerationCount;

			AllTimeResults.Add(RunResult);

			// 4. LOG TO FILE (This creates EvolutionHistory.txt)
			TreeWrapper->AppendTreeToLogFile(TEXT("EvolutionHistory.txt"), GenerationCount, FitnessScore);
            
			// Cleanup
			if (AController* C = Agent->GetController()) C->Destroy();
			Agent->Destroy();
		}
	}
    
	// Clear list for next run
	ActiveAgents.Empty();
    
	GenerationCount++;
	FinishEpoch();
}

void UGeneticSimulationManager::SetPause(bool bPauseState)
{
    if (TargetWorld) UGameplayStatics::SetGamePaused(TargetWorld, bPauseState);
}

bool UGeneticSimulationManager::DoesPlayerExist() const
{
    return UGameplayStatics::GetPlayerCharacter(TargetWorld, 0) != nullptr;
}

UClass* UGeneticSimulationManager::GetClassFromPath(const FString& Path)
{
    return StaticLoadClass(UObject::StaticClass(), nullptr, *Path);
}

UObject* UGeneticSimulationManager::LoadObjectFromPath(const FString& Path)
{
    return StaticLoadObject(UObject::StaticClass(), nullptr, *Path);
}

bool UGeneticSimulationManager::IsPaused() const
{
    return UGameplayStatics::IsGamePaused(TargetWorld);
}