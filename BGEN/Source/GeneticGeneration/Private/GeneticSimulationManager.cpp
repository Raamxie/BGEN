#include "GeneticSimulationManager.h"
#include "GeneticGenerationModule.h"
#include "CustomAIController.h"
#include "CustomBehaviourTree.h"
#include "GeneticExchangeLibrary.h"
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
#include "NavigationSystem.h"

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

	// 1. SETUP IDENTITY
	// Check if ID is passed via command line (e.g. -InstanceID=1)
	FString CmdID;
	if (FParse::Value(FCommandLine::Get(), TEXT("InstanceID="), CmdID))
	{
		InstanceID = TEXT("Island_") + CmdID;
	}
	else
	{
		// Fallback to random if running locally/testing
		InstanceID = TEXT("Island_") + FGuid::NewGuid().ToString().Left(4);
	}
	UE_LOG(LogGeneticGeneration, Warning, TEXT("MANAGER: Initialized as %s"), *InstanceID);
}

void UGeneticSimulationManager::StartEpoch()
{
	UE_LOG(LogGeneticGeneration, Display, TEXT("--------------------------------------------------"));
	UE_LOG(LogGeneticGeneration, Display, TEXT("MANAGER: Starting Generation %d"), GenerationCount);

	if (!TargetWorld) return;

	// Cleanup
	if (TimerHandle.IsValid()) TargetWorld->GetTimerManager().ClearTimer(TimerHandle);
	EnemySpawnPositions.Empty(); 
	
	// *** 2. EXCHANGE STEP: LOOK FOR NEW FOREIGN ASSETS ***
	// Scan the shared folder
	TArray<FSimulationResult> NewForeigners = UGeneticExchangeLibrary::ScanForForeignGenomes(InstanceID);
	
	if (NewForeigners.Num() > 0)
	{
		// Add to our foreign pool
		ForeignPool.Append(NewForeigners);
		
		// Clean up pool if too large (keep best 100)
		if (ForeignPool.Num() > 100)
		{
			ForeignPool.Sort([](const FSimulationResult& A, const FSimulationResult& B) {
				return A.Fitness > B.Fitness; 
			});
			ForeignPool.SetNum(100);
		}
	}

	// Setup Arena
	SetPause(true);

	if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(TargetWorld))
	{
		UE_LOG(LogGeneticGeneration, Log, TEXT("Forcing Synchronous NavMesh Build..."));
		NavSys->Build();
	}
	
	PreparePlayer();
	
	// Pass seed path (only used if no history exists)
	FString SeedPath = TEXT("/Game/Actors/EnemyUnleashed/Test");
	SpawnEnemies(1, SeedPath); 
	
	Simulate();
}

FString UGeneticSimulationManager::SelectTreeToEvolve()
{
	// 1. Create a "Mega Pool" of Local History + Foreign Discoveries
	TArray<FSimulationResult> MegaPool = AllTimeResults;
	MegaPool.Append(ForeignPool);

	if (MegaPool.Num() == 0)
	{
		return TEXT("/Game/Actors/EnemyUnleashed/Test"); 
	}

	// 2. Filter valid
	TArray<FSimulationResult> Candidates;
	for (const FSimulationResult& Result : MegaPool)
	{
		if (Result.Fitness > 0.0f && !Result.BehaviorTreePath.IsEmpty())
		{
			Candidates.Add(Result);
		}
	}

	if (Candidates.Num() > 0)
	{
		// 3. Tournament Selection on the mixed pool
		FSimulationResult Winner = UGeneticSelectionLibrary::TournamentSelection(Candidates, 3);
		
		// Logging source
		if (Winner.BehaviorTreePath.Contains("Exchange"))
		{
			UE_LOG(LogGeneticGeneration, Display, TEXT("Selection: Crossbreeding with FOREIGN genome: %s"), *Winner.BehaviorTreePath);
		}
		else
		{
			UE_LOG(LogGeneticGeneration, Display, TEXT("Selection: Local ancestor chosen: %s"), *Winner.BehaviorTreePath);
		}

		return Winner.BehaviorTreePath;
	}
	else
	{
		// Fallback
		int32 RandIndex = FMath::RandRange(0, MegaPool.Num() - 1);
		return MegaPool[RandIndex].BehaviorTreePath;
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

	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	AActor* SpawnPoint = nullptr;

	// OPTION 1: Find by Blueprint Class Path (Replace with your actual path, remember the _C at the end!)
	UClass* SpawnPointClass = GetClassFromPath("/Game/Actors/EnemyUnleashed/EnemySpawn.EnemySpawn_C");
	if (SpawnPointClass)
	{
		SpawnPoint = UGameplayStatics::GetActorOfClass(TargetWorld, SpawnPointClass);
	}

	if (SpawnPoint)
	{
		SpawnLocation = SpawnPoint->GetActorLocation();
		SpawnRotation = SpawnPoint->GetActorRotation();
	}
	else
	{
		UE_LOG(LogGeneticGeneration, Warning, TEXT("SpawnEnemies: Could not find BP_EnemySpawn in the world! Using fallback."));
	}

    for (int32 i = 0; i < AmountToSpawn; i++)
    {
        // 2. Spawn Logic
    	FVector Loc = SpawnPoint ? SpawnLocation : ((EnemySpawnPositions.IsValidIndex(i)) ? EnemySpawnPositions[i] : FVector(i * 150.0f, 1500.0f, 210.0f));
    	FRotator Rot = SpawnPoint ? SpawnRotation : FRotator::ZeroRotator;

    	FActorSpawnParameters SpawnParams;
    	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    	
    	ACharacter* Enemy = TargetWorld->SpawnActor<ACharacter>(EnemyClass, Loc, Rot, SpawnParams);
    	ACustomAIController* AI = TargetWorld->SpawnActor<ACustomAIController>(ControllerClass, Loc, Rot, SpawnParams);

        if (!Enemy || !AI) continue;

        // Attach Fitness Tracker
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
                // LOAD PARENT A
                UCustomBehaviourTree* WrapperA = NewObject<UCustomBehaviourTree>(this);
                if (WrapperA->LoadBehaviorTree(ParentA.BehaviorTreePath))
                {
                    // *** CAPTURE PARENT A STRUCTURE (For Log) ***
                    History.ParentA_Path = ParentA.BehaviorTreePath;
                    History.ParentA_Structure = WrapperA->GetTreeAsString();
                    
                    // Crossover Logic (70% Chance)
                    bool bCrossoverSuccess = false;
                    if (UGeneticSelectionLibrary::IsValidResult(ParentB) && FMath::FRand() < 0.7f) 
                    {
                        // LOAD PARENT B
                        UCustomBehaviourTree* WrapperB = NewObject<UCustomBehaviourTree>(this);
                        if(WrapperB->LoadBehaviorTree(ParentB.BehaviorTreePath))
                        {
                             // *** CAPTURE PARENT B STRUCTURE (For Log) ***
                             History.ParentB_Path = ParentB.BehaviorTreePath;
                             History.ParentB_Structure = WrapperB->GetTreeAsString();

                             FString Log;
                             ChildWrapper = WrapperA->PerformCrossover(WrapperB, Log);
                             History.CrossoverLog = Log;
                             bCrossoverSuccess = true;
                        }
                    }
                    
                    // Fallback: Clone Parent A if Crossover failed or skipped
                    if (!bCrossoverSuccess)
                    {
                        ChildWrapper = NewObject<UCustomBehaviourTree>(this);
                        ChildWrapper->InitFromTreeInstance(WrapperA->GetBTAsset());
                        History.CrossoverLog = TEXT("Clone (No Crossover)");
                    }
                }
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
                History.ParentA_Structure = ChildWrapper->GetTreeAsString(); // Capture Seed Structure
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
            bool bUniqueFound = false;
            int32 MaxRetries = 20; // Failsafe to prevent infinite engine hangs

            // Keep trying to mutate until we find a tree we haven't evaluated yet
            for (int32 Attempt = 0; Attempt < MaxRetries; ++Attempt)
            {
                // BOOTSTRAP: If Seed (Gen 0) on the very FIRST attempt
                if (bIsSeed && Attempt == 0)
                {
                    // Mutate 5 times to generate a complex tree immediately
                    for(int k=0; k<5; k++)
                    {
                        FString StepLog = UGeneticMutationLibrary::MutateTree(ChildWrapper, 1.0f);
                        if (!StepLog.IsEmpty()) MLog += StepLog + " | ";
                    }
                    History.MutationLog = TEXT("BOOTSTRAP: ") + MLog;
                }
                else
                {
                    // If it's a retry, we FORCE a mutation (1.0f) so it doesn't get stuck checking the same tree.
                    // If it's the first standard attempt, use the normal 40% chance.
                    float MutRate = (Attempt == 0) ? 0.40f : 1.0f;
                    FString StepLog = UGeneticMutationLibrary::MutateTree(ChildWrapper, MutRate);
                    
                    if (Attempt == 0)
                    {
                        History.MutationLog = StepLog.IsEmpty() ? TEXT("No Mutation") : StepLog;
                    }
                    else if (!StepLog.IsEmpty())
                    {
                        // Append retry logs so we can see the path it took to become unique
                        History.MutationLog += FString::Printf(TEXT(" -> [Retry %d] %s"), Attempt, *StepLog);
                    }
                }

                // --- DUPLICATE HASH CHECK ---
                FString NewTreeHash = ChildWrapper->GetTreeHash();
                FString ExistingAssetPath;
                float KnownFitness = UGeneticExchangeLibrary::CheckIfTreeAlreadyEvaluated(NewTreeHash, ExistingAssetPath);

                if (KnownFitness < 0.0f)
                {
                    // We found a completely unique, unevaluated tree! Break the loop.
                    bUniqueFound = true;
                    break; 
                }
                
            }

            // If we maxed out our retries and STILL didn't find a unique tree, destroy it to save CPU
            if (!bUniqueFound)
            {
                UE_LOG(LogGeneticGeneration, Error, TEXT("Agent %d failed to generate a unique tree after %d attempts! Skipping..."), i, MaxRetries);
                if (AI) AI->Destroy(); 
                if (Enemy) Enemy->Destroy(); 
                continue; // Skip adding to ActiveAgents!
            }
            // ----------------------------------

            // Save History & Assign
            ChildWrapper->EvolutionData = History;
            AI->AssignTree(ChildWrapper->GetBTAsset(), BBAsset);

        	AI->RuntimeBehaviourWrapper = ChildWrapper;
            
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

void UGeneticSimulationManager::StopSimulation()
{
	if (TargetWorld && TimerHandle.IsValid())
	{
		TargetWorld->GetTimerManager().ClearTimer(TimerHandle);
	}
	SetPause(true);

	float BestFitnessOfGen = -1.0f;

	UE_LOG(LogGeneticGeneration, Log, TEXT("Manager: Stopping Simulation. Processing Agents..."));

	for (auto& Pair : ActiveAgents)
	{
		APawn* Agent = Pair.Key;
		UCustomBehaviourTree* TreeWrapper = Pair.Value;

		if (IsValid(Agent) && TreeWrapper)
		{
			// 1. Calc Fitness
			float FitnessScore = 0.0f;
			if (UGeneticFitnessTracker* Tracker = Agent->FindComponentByClass<UGeneticFitnessTracker>())
			{
				FitnessScore = Tracker->CalculateFitness();
			}

			// 2. Save using Hash instead of Random ID
			FString TreeHash = TreeWrapper->GetTreeHash();
			int32 FitInt = FMath::RoundToInt(FitnessScore);
            
			// Format: Tree_{Hash}_F{Fitness}
			FString SaveName = FString::Printf(TEXT("/Game/BehaviourTrees/Generated/Tree_%s_F%d"), *TreeHash, FitInt);
			FString FinalAssetPath = TreeWrapper->SaveAsNewAsset(SaveName, true);
			
			// 3. Track Best (Useful for local console logging if needed)
			if (FitnessScore > BestFitnessOfGen)
			{
				BestFitnessOfGen = FitnessScore;
			}

			// 4. Record Result Locally
			FSimulationResult RunResult;
			RunResult.BehaviorTreePath = FinalAssetPath;
			RunResult.Fitness = FitnessScore;
			RunResult.GenerationNumber = GenerationCount;

			AllTimeResults.Add(RunResult);
			TreeWrapper->AppendTreeToLogFile(TEXT("EvolutionHistory.txt"), GenerationCount, FitnessScore);
			
			// Cleanup
			if (AController* C = Agent->GetController()) C->Destroy();
			Agent->Destroy();
		}
	}

	ActiveAgents.Empty();
	GenerationCount++;
	FinishEpoch();
}

UWorld* UGeneticSimulationManager::GetWorld() const { return TargetWorld; }
void UGeneticSimulationManager::FinishEpoch() { if (OnEpochComplete.IsBound()) OnEpochComplete.Broadcast(); }
void UGeneticSimulationManager::SetPause(bool bPauseState) { if (TargetWorld) UGameplayStatics::SetGamePaused(TargetWorld, bPauseState); }
bool UGeneticSimulationManager::IsPaused() const { return UGameplayStatics::IsGamePaused(TargetWorld); }
bool UGeneticSimulationManager::DoesPlayerExist() const { return UGameplayStatics::GetPlayerCharacter(TargetWorld, 0) != nullptr; }
UClass* UGeneticSimulationManager::GetClassFromPath(const FString& Path) { return StaticLoadClass(UObject::StaticClass(), nullptr, *Path); }
UObject* UGeneticSimulationManager::LoadObjectFromPath(const FString& Path) { return StaticLoadObject(UObject::StaticClass(), nullptr, *Path); }
void UGeneticSimulationManager::TimerCallback() { UE_LOG(LogGeneticGeneration, Warning, TEXT("TIMEOUT")); StopSimulation(); }
void UGeneticSimulationManager::PlayerDiedListener() { UE_LOG(LogGeneticGeneration, Display, TEXT("PLAYER DIED")); StopSimulation(); }