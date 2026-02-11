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

UGeneticSimulationManager::UGeneticSimulationManager()
{
}

void UGeneticSimulationManager::Init(UWorld* InWorld)
{
    TargetWorld = InWorld;
    
    // Ensure determinism settings are applied every run
	if (GEngine)
	{
		GEngine->bSmoothFrameRate = false; 
		GEngine->bUseFixedFrameRate = true; 
		GEngine->FixedFrameRate = 60.0f;    
	}
}

// THIS IS THE NEW MAIN FUNCTION
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
	FString ChosenTreePath = SelectTreeToEvolve();

	// 3. EXECUTE: Setup the arena
	SetPause(true);
	PreparePlayer();
	
    // 4. SPAWN: Pass the chosen gene to the spawner
	SpawnEnemies(1, ChosenTreePath); 
	
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

// ... [Keep GetWorld, FinishEpoch, PreparePlayer as they were] ...

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

    UClass* EnemyClass = GetClassFromPath("/Game/Actors/EnemyUnleashed/EnemyUnleashed.EnemyUnleashed_C");
    UClass* ControllerClass = GetClassFromPath("/Game/Actors/EnemyUnleashed/AIController_EnemyUnleashed.AIController_EnemyUnleashed_C");
    UBlackboardData* BBAsset = Cast<UBlackboardData>(LoadObjectFromPath("/Game/Actors/EnemyUnleashed/BB_EnemyUnleashed.BB_EnemyUnleashed"));

    if (!EnemyClass || !ControllerClass || !BBAsset) return;

    for (int32 i = 0; i < AmountToSpawn; i++)
    {
        FVector Loc = (EnemySpawnPositions.IsValidIndex(i)) ? EnemySpawnPositions[i] : FVector(i * 150.0f, 0.0f, 210.0f);
        
        ACharacter* Enemy = TargetWorld->SpawnActor<ACharacter>(EnemyClass, Loc, FRotator::ZeroRotator);
        if (!Enemy) continue;

        UGeneticFitnessTracker* FitnessComp = NewObject<UGeneticFitnessTracker>(Enemy);
        if (FitnessComp)
        {
            FitnessComp->RegisterComponent(); 
            Enemy->AddInstanceComponent(FitnessComp);
        }

        ACustomAIController* AI = TargetWorld->SpawnActor<ACustomAIController>(ControllerClass, Loc, FRotator::ZeroRotator);
        if (AI)
        {
            AI->Possess(Enemy);
            
            UCustomBehaviourTree* GenHelper = NewObject<UCustomBehaviourTree>(this);
            bool bLoaded = GenHelper->LoadBehaviorTree(GenomePath);

            if (bLoaded)
            {
				GenHelper->MutateTree_Dynamic("/Game/BehaviourTrees");
				UBehaviorTree* EvolvedTree = GenHelper->GetBTAsset();
				AI->AssignTree(EvolvedTree, BBAsset);
				ActiveAgents.Add(Enemy, GenHelper);
            	GenHelper->DebugLogTree();
            }
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

    for (auto& Pair : ActiveAgents)
    {
        APawn* Agent = Pair.Key;
        UCustomBehaviourTree* TreeWrapper = Pair.Value;

        if (IsValid(Agent) && TreeWrapper)
        {
            FString SaveName = FString::Printf(TEXT("/Game/BehaviourTrees/Generated/Gen%d_Agent_%d"), GenerationCount, 1);
            FString FinalAssetPath = TreeWrapper->SaveAsNewAsset(SaveName, true);
            
            float FitnessScore = 0.0f;
            if (UGeneticFitnessTracker* Tracker = Agent->FindComponentByClass<UGeneticFitnessTracker>())
            {
                FitnessScore = Tracker->CalculateFitness();
            }

            FSimulationResult RunResult;
            RunResult.BehaviorTreePath = FinalAssetPath;
            RunResult.Fitness = FitnessScore;
            RunResult.GenerationNumber = GenerationCount;

            AllTimeResults.Add(RunResult);

        	TreeWrapper->AppendTreeToLogFile(TEXT("EvolutionHistory.txt"), GenerationCount, FitnessScore);
            
            if (AController* C = Agent->GetController()) C->Destroy();
            Agent->Destroy();
        }
    }
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