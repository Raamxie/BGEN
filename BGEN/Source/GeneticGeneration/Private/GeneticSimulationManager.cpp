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
#include "HttpModule.h"
#include "SimulationEventManager.h"
#include "WorkerNetworkSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

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
	FString CmdID;
	if (FParse::Value(FCommandLine::Get(), TEXT("InstanceID="), CmdID))
	{
		InstanceID = TEXT("Island_") + CmdID;
	}
	else
	{
		InstanceID = TEXT("Island_") + FGuid::NewGuid().ToString().Left(4);
	}
	UE_LOG(LogGeneticGeneration, Warning, TEXT("MANAGER: Initialized as %s"), *InstanceID);

	// 2. IMMEDIATELY FREEZE THE WORLD
	// Ensure the game does not start running/ticking while we poll the Master Server.
	SetPause(true); 
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
        UE_LOG(LogGeneticGeneration, Error, TEXT("SpawnEnemies: Missing Core Assets!"));
        return;
    }

    UE_LOG(LogGeneticGeneration, Log, TEXT("--- WORKER: SPAWNING JOB (%d Agents) ---"), AmountToSpawn);

	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	AActor* SpawnPoint = nullptr;

	// Locate the Spawn Point
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
		UE_LOG(LogGeneticGeneration, Warning, TEXT("SpawnEnemies: Could not find BP_EnemySpawn in the world! Using fallback loc."));
	}

    for (int32 i = 0; i < AmountToSpawn; i++)
    {
        // 2. Spawn the Character and Controller
    	FVector Loc = SpawnPoint ? SpawnLocation : ((EnemySpawnPositions.IsValidIndex(i)) ? EnemySpawnPositions[i] : FVector(i * 150.0f, 1500.0f, 210.0f));
    	FRotator Rot = SpawnPoint ? SpawnRotation : FRotator::ZeroRotator;

    	FActorSpawnParameters SpawnParams;
    	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    	
    	ACharacter* Enemy = TargetWorld->SpawnActor<ACharacter>(EnemyClass, Loc, Rot, SpawnParams);
    	ACustomAIController* AI = TargetWorld->SpawnActor<ACustomAIController>(ControllerClass, Loc, Rot, SpawnParams);

        if (!Enemy || !AI) continue;

        // Attach the Fitness Tracker dynamically
        UGeneticFitnessTracker* FitnessComp = NewObject<UGeneticFitnessTracker>(Enemy);
        FitnessComp->RegisterComponent();
        Enemy->AddInstanceComponent(FitnessComp);
        
        AI->Possess(Enemy);

        // 3. GENOME LOGIC - DUMB WORKER MODE
        // Instead of crossbreeding, we simply instantiate our wrapper and load the path given by the Master.
        // We use 'this' (the Manager) as the Outer to ensure the wrapper isn't garbage collected mid-simulation.
        UCustomBehaviourTree* AssignedWrapper = NewObject<UCustomBehaviourTree>(this);
        
        if (!AssignedWrapper->LoadBehaviorTree(GenomePath))
        {
            UE_LOG(LogGeneticGeneration, Error, TEXT("WORKER FATAL: Failed to load assigned tree: %s"), *GenomePath);
            AI->Destroy(); 
            Enemy->Destroy(); 
            continue;
        }

       // 4. ASSIGNMENT TO AI
        if (AssignedWrapper->GetBTAsset())
        {
            // Inject the tree into the controller
            AI->AssignTree(AssignedWrapper->GetBTAsset(), BBAsset);
        	AI->RuntimeBehaviourWrapper = AssignedWrapper;
            
            // Track the agent so StopSimulation() can evaluate it
            ActiveAgents.Add(Enemy, AssignedWrapper);
            
            UE_LOG(LogGeneticGeneration, Log, TEXT("Worker successfully bound agent %d to Tree: %s"), i, *AssignedWrapper->GetBTAsset()->GetName());
        }
    }
}

void UGeneticSimulationManager::Simulate()
{
    UE_LOG(LogTemp, Warning, TEXT("MANAGER DIAGNOSTIC: Simulate() started. Active Agents: %d"), ActiveAgents.Num());

    if (!TargetWorld || ActiveAgents.Num() == 0)
    {
       UE_LOG(LogTemp, Error, TEXT("MANAGER DIAGNOSTIC: Simulate() aborted! ActiveAgents is empty or TargetWorld is null."));
       StopSimulation(); 
       return;
    }

    // 1. Bind to the Event Manager Subsystem
    USimulationEventManager* EventManager = TargetWorld->GetSubsystem<USimulationEventManager>();
    if (EventManager)
    {
       EventManager->OnSimulationEvent.RemoveAll(this);
       EventManager->OnSimulationEvent.AddUObject(this, &UGeneticSimulationManager::HandleSimulationEvent);
       
       UE_LOG(LogTemp, Warning, TEXT("MANAGER DIAGNOSTIC: SUCCESSFULLY BOUND DELEGATE to World: %s"), *TargetWorld->GetName());
    }
    else
    {
       UE_LOG(LogTemp, Error, TEXT("MANAGER DIAGNOSTIC: EventManager is NULL! Could not bind."));
    }

    // 2. Initialize Trackers and Start AI Execution
    for (const auto& Pair : ActiveAgents)
    {
        APawn* Agent = Pair.Key;
        if (IsValid(Agent))
        {
            // A. Start Fitness Tracking
            // We pass in the ActivePlayer so the tracker can measure proximity and damage events.
            if (UGeneticFitnessTracker* Tracker = Agent->FindComponentByClass<UGeneticFitnessTracker>())
            {
                Tracker->BeginTracking(ActivePlayer);
            }

            // B. Run the Behavior Tree
            // The tree was assigned in SpawnEnemies, but it must be explicitly started here.
            if (ACustomAIController* AIController = Cast<ACustomAIController>(Agent->GetController()))
            {
                AIController->RunAssignedTree();
            }
        }
    }

    SetPause(false);
}

void UGeneticSimulationManager::StopSimulation()
{
	if (TargetWorld && TimerHandle.IsValid())
	{
		TargetWorld->GetTimerManager().ClearTimer(TimerHandle);
	}
	SetPause(true);

	UE_LOG(LogGeneticGeneration, Log, TEXT("WORKER: Stopping Simulation. Gathering Metrics..."));

	float FinalFitnessScore = 1.0f; 
	float Dist = 0.0f, DmgTaken = 0.0f,DmgDealt = 0.0f, Rew = 0.0f, TimeAlive = 0.0f, Util = 0.0f;
	int32 Size = 0;
	bool bKilled = false;
	FString TreeStr = TEXT("No Tree Found");

	// 1. Calculate Fitness, Extract Metrics AND Destroy Agents!
	for (auto& Pair : ActiveAgents)
	{
		APawn* Agent = Pair.Key;
		if (IsValid(Agent))
		{
			// Extract all metrics from the tracker
			if (UGeneticFitnessTracker* Tracker = Agent->FindComponentByClass<UGeneticFitnessTracker>())
			{
				FinalFitnessScore = Tracker->CalculateFitness();
				Dist = Tracker->GetAccumulatedDistance();
				DmgTaken = Tracker->GetAccumulatedDamageTaken();
				DmgDealt = Tracker->GetAccumulatedDamageDealt();
				Rew = Tracker->GetAccumulatedReward();
				TimeAlive = Tracker->GetTimeAlive();
				Size = Tracker->GetTreeSize();
				Util = Tracker->GetTreeUtilizationPercentage();
				bKilled = Tracker->GetPlayerWasKilled();
			}
			
			// Extract the tree string and destroy the Controller
			if (ACustomAIController* C = Cast<ACustomAIController>(Agent->GetController()))
			{
				if (C->RuntimeBehaviourWrapper)
				{
					TreeStr = C->RuntimeBehaviourWrapper->GetTreeAsString();
				}
				C->Destroy();
			}
			
			// Destroy the Agent so it stops ticking/running completely
			Agent->Destroy();
		}
	}
	ActiveAgents.Empty();

	// 2. Submit Result to Server
	if (TargetWorld)
	{
		UGameInstance* GameInstance = TargetWorld->GetGameInstance();
		if (GameInstance)
		{
			UWorkerNetworkSubsystem* NetSubsystem = GameInstance->GetSubsystem<UWorkerNetworkSubsystem>();
			if (NetSubsystem)
			{
				UE_LOG(LogGeneticGeneration, Log, TEXT("WORKER: Submitting Job %d (Fitness: %.2f). Waiting for Server..."), NetSubsystem->CurrentJobID, FinalFitnessScore);
				
				NetSubsystem->SubmitFitness(
					NetSubsystem->CurrentJobAssetPath, 
					NetSubsystem->CurrentJobID, 
					FinalFitnessScore, 
					Dist, DmgTaken, DmgDealt, Rew, TimeAlive, Size, Util, bKilled, TreeStr
				);
			}
		}
	}
}
void UGeneticSimulationManager::StartEpochWithJob(FString AssignedAssetPath)
{
	// The Master has assigned us a tree! We can now run the standard setup.
	UE_LOG(LogGeneticGeneration, Display, TEXT("WORKER: Job Received! Starting Simulation for %s"), *AssignedAssetPath);

	if (!TargetWorld) return;

	if (TimerHandle.IsValid()) TargetWorld->GetTimerManager().ClearTimer(TimerHandle);
	EnemySpawnPositions.Empty(); 

	SetPause(true);

	if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(TargetWorld))
	{
		NavSys->Build();
	}
    
	PreparePlayer();
    
	// Pass the exact path from the Master directly into the spawner
	SpawnEnemies(1, AssignedAssetPath); 
    
	Simulate();
}

void UGeneticSimulationManager::HandleSimulationEvent(ESimulationEvent EventType)
{
	switch (EventType)
	{
	case ESimulationEvent::NoFoodLeft:
		UE_LOG(LogGeneticGeneration, Warning, TEXT("Stopping Simulation: Starvation."));
		StopSimulation();
		break;

	case ESimulationEvent::NoWaterLeft:
		UE_LOG(LogGeneticGeneration, Warning, TEXT("Stopping Simulation: Dehydration."));
		StopSimulation();
		break;

	case ESimulationEvent::SettlementComplete:
		UE_LOG(LogGeneticGeneration, Warning, TEXT("Stopping Simulation: Settlement Completed Successfully!"));
		StopSimulation();
		break;

	case ESimulationEvent::PlayerDied:
		UE_LOG(LogGeneticGeneration, Warning, TEXT("Stopping Simulation: Player was killed."));
		StopSimulation();
		break;

	case ESimulationEvent::Timeout:
		UE_LOG(LogGeneticGeneration, Warning, TEXT("Stopping Simulation: Time limit reached."));
		StopSimulation();
		break;
            
	default:
		break;
	}
}

void UGeneticSimulationManager::TransitionToMainMap(FString JobPath)
{
	UE_LOG(LogGeneticGeneration, Warning, TEXT("WORKER: Job received from server! Loading Main Map..."));
	
	if (TargetWorld)
	{
		// Map transition triggers the engine to load Main and destroy EmptyWaiting
		UGameplayStatics::OpenLevel(TargetWorld, FName("Main"));
	}
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