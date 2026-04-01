#include "GeneticFitnessTracker.h"
#include "GeneticGenerationModule.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "CustomAIController.h"
#include "CustomBehaviourTree.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTTaskNode.h"

UGeneticFitnessTracker::UGeneticFitnessTracker()
{
	PrimaryComponentTick.bCanEverTick = false; 
}

void UGeneticFitnessTracker::BeginPlay()
{
	Super::BeginPlay();
}
void UGeneticFitnessTracker::BeginTracking(AActor* InTargetPlayer)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	Owner->OnTakeAnyDamage.RemoveDynamic(this, &UGeneticFitnessTracker::OnOwnerTakeDamage);
	Owner->OnTakeAnyDamage.AddDynamic(this, &UGeneticFitnessTracker::OnOwnerTakeDamage);

	TargetPlayer = InTargetPlayer;

	APlayerUnleashedBase* PlayerUnleashed = Cast<APlayerUnleashedBase>(TargetPlayer);
	if (PlayerUnleashed)
	{

		PlayerUnleashed->OnTakeAnyDamage.RemoveDynamic(this, &UGeneticFitnessTracker::OnPlayerTakeDamage);
		PlayerUnleashed->OnTakeAnyDamage.AddDynamic(this, &UGeneticFitnessTracker::OnPlayerTakeDamage);
	}

	if (USimulationEventManager* SimEventManager = GetWorld()->GetSubsystem<USimulationEventManager>())
	{
		// Clear previous bindings to avoid duplicates on multiple runs
		SimEventManager->OnSimulationEvent.RemoveAll(this); 
		SimEventManager->OnSimulationEvent.AddUObject(this, &UGeneticFitnessTracker::OnGlobalSimulationEvent);
	}

	APawn* OwnerPawn = Cast<APawn>(Owner);
	if (OwnerPawn)
	{
		CachedAIController = Cast<ACustomAIController>(OwnerPawn->GetController());
	}

	LastRecordedLocation = Owner->GetActorLocation();
	GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &UGeneticFitnessTracker::RecordMovementRoutine, 0.5f, true);

	StartTime = GetWorld()->GetTimeSeconds();
	bTrackingActive = true;
	bPlayerWasKilled = false;
	bDamagedPlayer = false;
	AccumulatedReward = 0.0f;
	AccumulatedDamageTaken = 0.0f;
	AccumulatedDamageDealt = 0.0f;
	AccumulatedDistance = 0.0f;
    
	// Clear the set for a fresh run
	ExecutedTasks.Empty();

	// Initialize Grace Period
	bIsGracePeriodActive = (TrackingGracePeriod > 0.0f);
	if (bIsGracePeriodActive)
	{
		GetWorld()->GetTimerManager().SetTimer(GracePeriodTimerHandle, this, &UGeneticFitnessTracker::EndGracePeriod, TrackingGracePeriod, false);
	}
}

void UGeneticFitnessTracker::RecordNodeExecution(UBTTaskNode* Node)
{
	if (!Node) return;
    
	if (bTrackingActive && !bIsGracePeriodActive)
	{
		ExecutedTasks.Add(Node);
	}
}

void UGeneticFitnessTracker::ReportTaskExecuted(AAIController* AIController, UBTTaskNode* TaskNode)
{
	if (!AIController || !TaskNode) return;

	APawn* ControlledPawn = AIController->GetPawn();
	if (ControlledPawn)
	{
		if (UGeneticFitnessTracker* Tracker = ControlledPawn->FindComponentByClass<UGeneticFitnessTracker>())
		{
			// We pass the actual memory pointer of the node!
			Tracker->RecordNodeExecution(TaskNode);
		}
	}
}

float UGeneticFitnessTracker::GetTreeUtilizationPercentage() const
{
	int32 TotalSize = GetTreeSize();
	if (TotalSize == 0) return 0.0f;

	return (float)ExecutedTasks.Num() / (float)TotalSize;
}

void UGeneticFitnessTracker::RecordMovementRoutine()
{
	if (!bTrackingActive) return;

	AActor* Owner = GetOwner();
	if (Owner)
	{
		FVector CurrentLocation = Owner->GetActorLocation();
		AccumulatedDistance += FVector::Dist2D(LastRecordedLocation, CurrentLocation);
		LastRecordedLocation = CurrentLocation;
	}
}

void UGeneticFitnessTracker::OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!bTrackingActive) return;
	AccumulatedDamageTaken += Damage;
}

void UGeneticFitnessTracker::OnPlayerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!bTrackingActive) return;

	// Check if the damage was instigated by THIS AI's Controller
	bool bIsOurDamage = false;

	if (InstigatedBy && InstigatedBy == CachedAIController)
	{
		bIsOurDamage = true;
	}
	// Fallback: If it's a melee attack and the Character itself is the causer
	else if (DamageCauser && DamageCauser == GetOwner())
	{
		bIsOurDamage = true;
	}

	if (bIsOurDamage)
	{
		AccumulatedDamageDealt += Damage;
		AccumulatedReward += (Damage * DamageDealtWeight) + SuccessfulAttackBonus;
		bDamagedPlayer = true;
	}
}

void UGeneticFitnessTracker::OnPlayerDied()
{
	if (!bTrackingActive) return;
	bPlayerWasKilled = true;
}

void UGeneticFitnessTracker::AddCustomReward(float Amount)
{
	if (bTrackingActive) AccumulatedReward += Amount;
}

int32 UGeneticFitnessTracker::GetTreeSize() const
{
	if (CachedAIController && CachedAIController->RuntimeBehaviourWrapper && CachedAIController->RuntimeBehaviourWrapper->GetBTAsset())
	{
		TArray<UBTCompositeNode*> Composites;
		TArray<UBTTaskNode*> Tasks;
		CachedAIController->RuntimeBehaviourWrapper->CollectNodes(CachedAIController->RuntimeBehaviourWrapper->GetBTAsset()->RootNode, Composites, Tasks);
		
		return Tasks.Num(); // Return ONLY Leaf Tasks
	}
	return 0; 
}

float UGeneticFitnessTracker::CalculateFitness()
{
	if (!bTrackingActive && AccumulatedReward == 0.0f) return 1.0f;

	bTrackingActive = false;
	
	// Clear both timers safely
	GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(GracePeriodTimerHandle);

	double CurrentTime = GetWorld()->GetTimeSeconds();
	float TimeAlive = (float)(CurrentTime - StartTime);

	// 1. BASE REWARDS 
	float FinalScore = BaseFitnessScore + AccumulatedReward;
	FinalScore += (TimeAlive * SurvivalTimeWeight);
	FinalScore += (AccumulatedDistance * DistanceMovedWeight);

	if (IsValid(TargetPlayer) && IsValid(GetOwner()))
	{
		float FinalDistanceToPlayer = FVector::Dist(TargetPlayer->GetActorLocation(), GetOwner()->GetActorLocation());
		float ProximityScore = FMath::Max(0.0f, 2000.0f - FinalDistanceToPlayer); 
		FinalScore += (ProximityScore * ProximityBonusWeight);
	}

	// 2. PENALTIES
	FinalScore -= (AccumulatedDamageTaken * DamageTakenPenalty);

	if (!bDamagedPlayer) FinalScore -= PacifistPenalty;
	if (AccumulatedDistance < MinimumExpectedDistance) FinalScore -= IdlePenalty;

	int32 TreeSize = GetTreeSize();
	if (TreeSize < MinimumTreeNodes) 
	{
		FinalScore -= SmallTreePenalty;
	}
	else if (TreeSize > MaximumTreeNodes) 
	{
		FinalScore -= BigTreePenalty;
	}

	// 3. VICTORY CONDITIONS
	if (bPlayerWasKilled)
	{
		FinalScore += PlayerKillBonus;
		float TimeRemaining = FMath::Max(0.0f, 30.0f - TimeAlive);
		FinalScore += (TimeRemaining * 50.0f); 
	}

	// --- LOGGING VISITED VS ALL NODES ---
	FString ExecutedNodesStr;
	FString AllNodesStr;
	
	// 1. Build the string for ALL tasks in the tree
	if (CachedAIController && CachedAIController->RuntimeBehaviourWrapper && CachedAIController->RuntimeBehaviourWrapper->GetBTAsset())
	{
		TArray<UBTCompositeNode*> Composites;
		TArray<UBTTaskNode*> Tasks;
		CachedAIController->RuntimeBehaviourWrapper->CollectNodes(CachedAIController->RuntimeBehaviourWrapper->GetBTAsset()->RootNode, Composites, Tasks);

		for (UBTTaskNode* Task : Tasks)
		{
			if (Task)
			{
				AllNodesStr += Task->GetClass()->GetName() + TEXT(", ");
			}
		}
	}

	// 2. Build the string for VISITED tasks directly from our tracking set
	for (UBTTaskNode* ExecTask : ExecutedTasks)
	{
		if (ExecTask)
		{
			ExecutedNodesStr += ExecTask->GetClass()->GetName() + TEXT(", ");
		}
	}

	// Clean up trailing commas
	if (ExecutedNodesStr.EndsWith(TEXT(", "))) ExecutedNodesStr.LeftChopInline(2);
	if (AllNodesStr.EndsWith(TEXT(", "))) AllNodesStr.LeftChopInline(2);

	UE_LOG(LogGeneticGeneration, Display, TEXT("--- TASK UTILIZATION REPORT ---"));
	UE_LOG(LogGeneticGeneration, Display, TEXT("ALL TASKS IN TREE: [%s]"), *AllNodesStr);
	UE_LOG(LogGeneticGeneration, Display, TEXT("VISITED TASKS:     [%s]"), *ExecutedNodesStr);
	// -----------------------------------------

	UE_LOG(LogGeneticGeneration, Display, TEXT("Enemy Fitness: %f (Dist: %.1f | TaskCount: %d | Utilization: %.2f%%)"), FinalScore, AccumulatedDistance, TreeSize, GetTreeUtilizationPercentage() * 100.0f);
	
	return FMath::Max(1.0f, FinalScore);
}

float UGeneticFitnessTracker::GetTimeAlive() const
{
	if (GetWorld() && StartTime > 0.0f)
	{
		return GetWorld()->GetTimeSeconds() - StartTime;
	}
	return 0.0f;
}



void UGeneticFitnessTracker::EndGracePeriod()
{
	bIsGracePeriodActive = false;
	UE_LOG(LogGeneticGeneration, Log, TEXT("Grace period ended. Now recording BT node executions."));
}


void UGeneticFitnessTracker::OnGlobalSimulationEvent(ESimulationEvent EventType)
{
	if (!bTrackingActive) return;

	if (EventType == ESimulationEvent::PlayerDied)
	{
		bPlayerWasKilled = true;
	}
}
