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
		GEngine->bUseFixedFrameRate = true; // FORCE DETERMINISM
		GEngine->FixedFrameRate = 60.0f;    // Lock to 60 FPS
	}
}

void UGeneticSimulationManager::OnLevelReload(UWorld* NewWorld)
{
	UE_LOG(LogGeneticGeneration, Display, TEXT("PERSISTENCE: Manager surviving into Generation %d"), GenerationCount);

	// 1. Update the World Context
	TargetWorld = NewWorld;

	// 2. Safety: Clear any old timers from the previous world
	if (TimerHandle.IsValid())
	{
		TargetWorld->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
	}

	// 3. Reset Transient State
	EnemySpawnPositions.Empty(); 
    
	// 4. Restart the Logic
	SetPause(true);
	PreparePlayer();
	SpawnEnemies(1); 
	Simulate();
}

UWorld* UGeneticSimulationManager::GetWorld() const
{
    return TargetWorld;
}

void UGeneticSimulationManager::FinishEpoch()
{
	UE_LOG(LogGeneticGeneration, Log, TEXT("Manager: Epoch Finished. Notifying Module..."));
	
	// Broadcast to the Module so it can reload the map
	if (OnEpochComplete.IsBound())
	{
		OnEpochComplete.Broadcast();
	}
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
        UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Player not found. Spawning..."));

        UClass* PlayerPawnClass = GetClassFromPath("/Game/Actors/PlayerUnleashed/PlayerUnleashed.PlayerUnleashed_C");
        if (!PlayerPawnClass) return;

        AActor* PlayerStart = UGameplayStatics::GetActorOfClass(TargetWorld, APlayerStart::StaticClass());
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
    }
}

void UGeneticSimulationManager::SpawnEnemies(int32 AmountToSpawn)
{
    UE_LOG(LogGeneticGeneration, Log, TEXT("DEBUG: SpawnEnemies called. Requesting %d enemies."), AmountToSpawn);

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
            GenHelper->LoadBehaviorTree("/Game/Actors/EnemyUnleashed/BT_EnemyUnleashed.BT_EnemyUnleashed");
            
            // TODO: Mutate here based on GenerationCount
            
            UBehaviorTree* FinalTree = GenHelper->GetBTAsset();
            if (FinalTree)
            {
                AI->AssignTree(FinalTree, BBAsset);
            }
            
            ActiveAgents.Add(Enemy, GenHelper);
        }
    }
}

void UGeneticSimulationManager::SetSpawnPositions(TArray<FVector> positions)
{
}

void UGeneticSimulationManager::Simulate()
{
	UE_LOG(LogGeneticGeneration, Display, TEXT("--------------------------------------------------"));
	UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Simulate() called."));

	if (!TargetWorld || ActiveAgents.Num() == 0)
	{
		UE_LOG(LogGeneticGeneration, Warning, TEXT("DEBUG: No Active Agents or World invalid. Stopping."));
		StopSimulation(); 
		return;
	}

	// --- FIX: KICKSTART AI BRAINS ---
	UE_LOG(LogGeneticGeneration, Log, TEXT("DEBUG: Activating AI Brains..."));
	for (auto& Pair : ActiveAgents)
	{
		APawn* Agent = Pair.Key;
		if (IsValid(Agent))
		{
			ACustomAIController* AI = Cast<ACustomAIController>(Agent->GetController());
			if (AI)
			{
				// This call was missing!
				AI->RunAssignedTree();
			}

			// Also start fitness tracking now
			UGeneticFitnessTracker* Tracker = Agent->FindComponentByClass<UGeneticFitnessTracker>();
			if (Tracker)
			{
				Tracker->BeginTracking();
			}
		}
	}
	// -------------------------------

	// Set Time Dilation (Speed up simulation if desired)
	UGameplayStatics::SetGlobalTimeDilation(TargetWorld, 1.0f); 

	// Set Safety Timer (Simulation Duration)
	float TimeoutGameSeconds = 30.0f;
	TargetWorld->GetTimerManager().SetTimer(
	   TimerHandle, 
	   this, 
	   &UGeneticSimulationManager::TimerCallback, 
	   TimeoutGameSeconds, 
	   false
	);
    
	SetPause(false);
	UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Simulation LIVE. Max Duration: %.2fs"), TimeoutGameSeconds);
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

    if (TargetWorld)
    {
        TargetWorld->GetTimerManager().ClearTimer(TimerHandle);
    }

    SetPause(true);

    TArray<FSimulationResult> GenerationResults;

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

            FSimulationResult Result;
            Result.BehaviorTree = TreeWrapper->GetBTAsset();
            Result.Fitness = FitnessScore;
            
            GenerationResults.Add(Result);
            
            // Clean up agent
            if (AController* C = Agent->GetController()) C->Destroy();
            Agent->Destroy();
        }
    }

    ActiveAgents.Empty();
	
    UE_LOG(LogGeneticGeneration, Display, TEXT("DEBUG: Collected %d results."), GenerationResults.Num());
	
    FinishEpoch();
}

void UGeneticSimulationManager::OnWarmupFinished()
{
    // Implementation for warmup logic if needed
}

void UGeneticSimulationManager::SetPause(bool bPauseState)
{
    if (TargetWorld)
    {
        UGameplayStatics::SetGamePaused(TargetWorld, bPauseState);
    }
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